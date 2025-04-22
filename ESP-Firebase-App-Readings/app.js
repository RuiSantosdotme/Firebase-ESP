// Complete Project Details
// ESP32: https://RandomNerdTutorials.com/esp32-firebase-web-app/
// ESP8266: https://RandomNerdTutorials.com/esp8266-nodemcu-firebase-web-app/

import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-database.js";

// Firebase configuration
const firebaseConfig = {
    apiKey: "REPLACE_WITH_YOUR_Firebase_CONFIGURATION",
    authDomain: "REPLACE_WITH_YOUR_Firebase_CONFIGURATION",
    databaseURL: "REPLACE_WITH_YOUR_Firebase_CONFIGURATION",
    projectId: "REPLACE_WITH_YOUR_Firebase_CONFIGURATION",
    storageBucket: "REPLACE_WITH_YOUR_Firebase_CONFIGURATION",
    messagingSenderId: "REPLACE_WITH_YOUR_Firebase_CONFIGURATION",
    appId: "REPLACE_WITH_YOUR_Firebase_CONFIGURATION"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Get a reference to the database
const database = getDatabase(app);

// Database Paths
const dataFloatPath = 'test/float';
const dataIntPath = 'test/int';
const dataStringPath = 'test/string';

// Get database references
const databaseFloatRef = ref(database, dataFloatPath);
const databaseIntRef = ref(database, dataIntPath);
const databaseStringRef = ref(database, dataStringPath);

// Variables to save database current values
let floatReading;
let intReading;
let stringReading;

// Attach listeners
onValue(databaseFloatRef, (snapshot) => {
    floatReading = snapshot.val();
    console.log("Float reading: " + floatReading);
    document.getElementById("reading-float").innerHTML = floatReading;
});

onValue(databaseIntRef, (snapshot) => {
    intReading = snapshot.val();
    console.log("Int reading: " + intReading);
    document.getElementById("reading-int").innerHTML = intReading;
});

onValue(databaseStringRef, (snapshot) => {
    stringReading = snapshot.val();
    console.log("String reading: " + stringReading);
    document.getElementById("reading-string").innerHTML = stringReading;
});
