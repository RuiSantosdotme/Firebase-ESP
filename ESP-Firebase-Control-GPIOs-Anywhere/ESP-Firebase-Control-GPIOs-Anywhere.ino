/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at our blog.
    - ESP32: https://RandomNerdTutorials.com/firebase-control-esp32-gpios/
    - ESP8266: https://RandomNerdTutorials.com/firebase-control-esp8266-nodemcu-gpios/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based in the RTDB Basic Example by Firebase-ESP-Client library by mobizt
  https://github.com/mobizt/Firebase-ESP-Client/blob/main/examples/RTDB/Basic/Basic.ino
*/

#include <Arduino.h>
#if defined(ESP32)
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.
#include <ArduinoJson.h>

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

SSL_CLIENT ssl_client, stream_ssl_client;

// Firebase components
FirebaseApp app;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client), streamClient(stream_ssl_client);
RealtimeDatabase Database;

// Timer variables for loop
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10 seconds in milliseconds

// Database  path (where the data is)
String listenerPath = "board1/outputs/digital/";

// Declare outputs
const int output1 = 12;
const int output2 = 13;
const int output3 = 14;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void setup(){
  Serial.begin(115200);

  // Declare pins as outputs
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);

  initWiFi();

  // Configure SSL client
  ssl_client.setInsecure();
  stream_ssl_client.setInsecure();
  #if defined(ESP32)
    ssl_client.setConnectionTimeout(1000);
    ssl_client.setHandshakeTimeout(5);
    stream_ssl_client.setConnectionTimeout(1000);
    stream_ssl_client.setHandshakeTimeout(5);
  #elif defined(ESP8266)
    ssl_client.setTimeout(1000); // Set connection timeout
    ssl_client.setBufferSizes(4096, 1024); // Set buffer sizes
    stream_ssl_client.setTimeout(1000); // Set connection timeout
    stream_ssl_client.setBufferSizes(4096, 1024); // Set buffer sizes
  #endif

  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);

  // Set a database listener
  streamClient.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");
  Database.get(streamClient, listenerPath, processData, true /* SSE mode (HTTP Streaming) */, "streamTask");
}

void loop(){
  // Maintain authentication and async tasks
  app.loop();

  // Check if authentication is ready
  if (app.ready()){
    //Do nothing - everything works with callback functions
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;
      Serial.printf("Program running for %lu\n", currentTime);        
    }
  }
}

void processData(AsyncResult &aResult){
  // Exits when no result available when calling from the loop.
  if (!aResult.isResult())
    return;

  if (aResult.isEvent()){
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
  }

  if (aResult.isDebug()){
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
  }

  if (aResult.isError()){
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
  }

  // When it receives data from the database
  if (aResult.available()){
    RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
    // we received data from the streaming client
    if (RTDB.isStream()) {
      Serial.println("----------------------------");
      Firebase.printf("task: %s\n", aResult.uid().c_str());
      Firebase.printf("event: %s\n", RTDB.event().c_str());
      Firebase.printf("path: %s\n", RTDB.dataPath().c_str());
      Firebase.printf("etag: %s\n", RTDB.ETag().c_str());
      Firebase.printf("data: %s\n", RTDB.to<const char *>());
      Firebase.printf("type: %d\n", RTDB.type());

      // RTDB.type = 6 means the result is a JSON : https://github.com/mobizt/FirebaseClient/blob/main/resources/docs/realtime_database_result.md#--realtime_database_data_type-type
      // You receive a JSON when you initialize the stream
      if (RTDB.type() == 6) {
        Serial.println(RTDB.to<String>());
        // Parse JSON
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, RTDB.to<String>());
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }
        // Iterate through JSON object
        for (JsonPair kv : doc.as<JsonObject>()) {
          int gpioPin = atoi(kv.key().c_str()); // Convert key (e.g., "12") to int
          bool state = kv.value().as<bool>();
          digitalWrite(gpioPin, state ? HIGH : LOW);
        }
      }

      // RTDB.type() = 4 means the result is a boolean
      // RTDB.type() = 1 means the result is an integer
      // learn more here: https://github.com/mobizt/FirebaseClient/blob/main/resources/docs/realtime_database_result.md#--realtime_database_data_type-type
      if (RTDB.type() == 4 || RTDB.type() == 1){
        // get the GPIO number
        int GPIO_number = RTDB.dataPath().substring(1).toInt();
        bool state = RTDB.to<bool>();
        digitalWrite(GPIO_number, state);
        Serial.println("Updating GPIO State");
      }

      // The stream event from RealtimeDatabaseResult can be converted to values as following.
      /*bool v1 = RTDB.to<bool>();
      int v2 = RTDB.to<int>();
      float v3 = RTDB.to<float>();
      double v4 = RTDB.to<double>();
      String v5 = RTDB.to<String>();
      Serial.println(v5); */
    }
    else{
        Serial.println("----------------------------");
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
  }
}
