/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp8266-nodemcu-firebase-realtime-database/
*********/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Network and Firebase credentials
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

#define Web_API_KEY "REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"
#define DATABASE_URL "REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL"
#define USER_EMAIL "REPLACE_WITH_FIREBASE_PROJECT_EMAIL_USER"
#define USER_PASS "REPLACE_WITH_FIREBASE_PROJECT_USER_PASS"

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
const unsigned long sendInterval = 10000; // 10 seconds in milliseconds

// Variables to save values from the database
int intValue;
float floatValue;
String stringValue;

void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)    {
    Serial.print(".");
    delay(300);
  }

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

      // GET VALUES FROM DATABASE (and save the data in a variable)
      int intValue =  Database.get<int>(aClient, "/test/int");
      check_and_print_value (intValue);

      float floatValue = Database.get<float>(aClient, "/test/float");
      check_and_print_value(floatValue);

      String stringValue = Database.get<String>(aClient, "/test/string");
      check_and_print_value(stringValue);

      Serial.println("Requested data from /test/int, /test/float, and /test/string");
    }
  }
}

template <typename T>
void check_and_print_value(T value){
  // To make sure that we actually get the result or error.
  if (aClient.lastError().code() == 0){
    Serial.print("Success, Value: ");
    Serial.println(value);
  }
  else {
    Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
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

  if (aResult.available()) {
    // Log the task and payload
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
  }
}
