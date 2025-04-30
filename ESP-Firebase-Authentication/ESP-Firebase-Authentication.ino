/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at our blog: https://RandomNerdTutorials.com/esp32-esp8266-firebase-authentication/
  Based on this example: https://github.com/mobizt/FirebaseClient/blob/main/examples/App/AppInitialization/UserAuth/UserAuth.ino
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

// Network and Firebase credentials
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

#define Web_API_KEY "REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"

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
AsyncResult dbResult;

bool taskComplete = false;

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
  #if defined(ESP32)
    ssl_client.setConnectionTimeout(1000);
    ssl_client.setHandshakeTimeout(5);
  #elif defined(ESP8266)
    ssl_client.setTimeout(1000); // Set connection timeout
    ssl_client.setBufferSizes(4096, 1024); // Set buffer sizes
  #endif

  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
}

void loop(){
  // Maintain authentication and async tasks
  app.loop();

  // Check if authentication is ready
  if (app.ready() && !taskComplete){
    taskComplete = true;
    // Print authentication info
    Serial.println("Authentication Information");
    Firebase.printf("User UID: %s\n", app.getUid().c_str());
    Firebase.printf("Auth Token: %s\n", app.getToken().c_str());
    Firebase.printf("Refresh Token: %s\n", app.getRefreshToken().c_str());
    print_token_type(app);
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
