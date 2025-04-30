/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at our blog: https://RandomNerdTutorials.com/esp32-esp8266-firebase-bme280-rtdb/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Network and Firebase credentials
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

#define Web_API_KEY "REPLACE_WITH_YOUR_PROJECT_API_KEY"
#define DATABASE_URL "REPLACE_WITH_YOUR_DATABASE_URL"
#define USER_EMAIL "REPLACE_WITH_THE_USER_EMAIL"
#define USER_PASSWORD "REPLACE_WITH_THE_USER_PASSWORD"

// User function
void processData(AsyncResult &aResult);

// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;

// Timer variables for sending data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;

// Variable to save USER UID
String uid;

// Variables to save database paths
String databasePath;
String tempPath;
String humPath;
String presPath;

// BME280 sensor
Adafruit_BME280 bme; // I2C
float temperature;
float humidity;
float pressure;

// Initialize BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BME280 Initialized with success");
}

void setup(){
  Serial.begin(115200);

  initBME();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)    {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  ssl_client.setInsecure();
  #if defined(ESP32)
    ssl_client.setConnectionTimeout(1000);
    ssl_client.setHandshakeTimeout(5);
  #elif defined(ESP8266)
    ssl_client.setTimeout(1000); // Set connection timeout
    ssl_client.setBufferSizes(4096, 1024); // Set buffer sizes
  #endif

  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
}

void loop(){
  // Maintain authentication and async tasks
  app.loop();

  // Check if authentication is ready
  if (app.ready()){

    // Periodic data sending every 10 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;
        
      // Get User UID
      Firebase.printf("User UID: %s\n", app.getUid().c_str());
      uid = app.getUid().c_str();
      databasePath = "UsersData/" + uid;

      // Update database path for sensor readings
      tempPath = databasePath + "/temperature"; // --> UsersData/<user_uid>/temperature
      humPath = databasePath + "/humidity"; // --> UsersData/<user_uid>/humidity
      presPath = databasePath + "/pressure"; // --> UsersData/<user_uid>/pressure

      // Get latest sensor readings
      temperature = bme.readTemperature();
      humidity = bme.readHumidity();
      pressure = bme.readPressure()/100.0F;
        
      Serial.println("Writing to: " + tempPath);

      Database.set<float>(aClient, tempPath, temperature, processData, "RTDB_Send_Temperature");
      Database.set<float>(aClient, humPath, humidity, processData, "RTDB_Send_Humidity");
      Database.set<float>(aClient, presPath, pressure, processData, "RTDB_Send_Pressure");

    }
  }
}

void processData(AsyncResult &aResult){
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}
