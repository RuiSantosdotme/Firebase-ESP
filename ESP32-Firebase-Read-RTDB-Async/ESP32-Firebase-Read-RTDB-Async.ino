/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-firebase-realtime-database/
*********/

#include <Arduino.h>
#include <WiFi.h>
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

// Timer variables for reading data every 10 seconds
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
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Configure SSL client
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);

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
    // Getting data every 10 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;

      // GET VALUES FROM DATABASE (using the callback async method method)
      // you can then get the values on the processData function as soon as the results are available
      Database.get(aClient, "/test/int", processData, false, "RTDB_GetInt");
      Database.get(aClient, "/test/float", processData, false, "RTDB_GetFloat");
      Database.get(aClient, "/test/string", processData, false, "RTDB_GetString");

      Serial.println("Requested data from /test/int, /test/float, and /test/string");
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

  // here you get the values from the database and save them in variables if you need to use them later
  if (aResult.available())    {
    // Log the task and payload
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());

    // Extract the payload as a String
    String payload = aResult.c_str();

    /// Handle int from /test/int
    if (aResult.uid() == "RTDB_GetInt"){
      // Extract the value as an int
      intValue = payload.toInt();
      Firebase.printf("Stored intValue: %d\n", intValue);
    }
    // Handle float from /test/float
    else if (aResult.uid() == "RTDB_GetFloat"){
      // Extract the value as a float
      floatValue = payload.toFloat();
      Firebase.printf("Stored floatValue: %.2f\n", floatValue);
    }
        
    // Handle String from /test/string
    else if (aResult.uid() == "RTDB_GetString"){
      // Extract the value as a String
      stringValue = payload;
      Firebase.printf("Stored stringValue: %s\n", stringValue.c_str());
    }
  }
}
