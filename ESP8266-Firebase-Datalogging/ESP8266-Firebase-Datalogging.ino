/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at our blog: https://RandomNerdTutorials.com/esp8266-data-logging-firebase-realtime-database/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "time.h"

// Network and Firebase credentials
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

#define Web_API_KEY "REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"
#define DATABASE_URL "REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL"
#define USER_EMAIL "REPLACE_WITH_FIREBASE_PROJECT_EMAIL_USER"
#define USER_PASS "REPLACE_WITH_FIREBASE_PROJECT_USER_PASS"

// User functions
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
const unsigned long sendInterval = 10000; // 10 seconds in milliseconds

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String humPath = "/humidity";
String presPath = "/pressure";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;

const char* ntpServer = "pool.ntp.org";

// BME280 sensor
Adafruit_BME280 bme; // I2C
float temperature;
float humidity;
float pressure;

// Create JSON objects for storing data
object_t jsonData, obj1, obj2, obj3, obj4;
JsonWriter writer;

// Initialize BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup(){
  Serial.begin(115200);

  initBME();
  initWiFi();
  configTime(0, 0, ntpServer);

  // Configure SSL client
  ssl_client.setInsecure();
  ssl_client.setTimeout(1000); // Set connection timeout
  ssl_client.setBufferSizes(4096, 1024); // Set buffer sizes

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

      uid = app.getUid().c_str();

      // Update database path
      databasePath = "/UsersData/" + uid ;

      //Get current timestamp
      timestamp = getTime();
      Serial.print ("time: ");
      Serial.println (timestamp);

      parentPath= databasePath + "/" + String(timestamp);

      // Get sensor readings
      temperature = bme.readTemperature();
      humidity = bme.readHumidity();
      pressure = bme.readPressure()/100.0;

      // Create a JSON object with the data
      writer.create(obj1, tempPath, temperature);
      writer.create(obj2, humPath, humidity);
      writer.create(obj3, presPath, pressure);
      writer.create(obj4, timePath, timestamp);
      writer.join(jsonData, 4, obj1, obj2, obj3, obj4);

      Database.set<object_t>(aClient, parentPath, jsonData, processData, "RTDB_Send_Data");
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
