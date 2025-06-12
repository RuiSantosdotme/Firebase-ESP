import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-app.js";
import { getAuth } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";
import { getDatabase, ref, onValue, set } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-database.js";

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
const auth = getAuth(app);
const database = getDatabase(app);

// Export auth for use in auth.js
export { auth };

// DOM elements
const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");

// Elements for GPIO states
const stateElement1 = document.getElementById("state1");
const stateElement2 = document.getElementById("state2");
const stateElement3 = document.getElementById("state3");

// Button Elements
const btn1On = document.getElementById('btn1On');
const btn1Off = document.getElementById('btn1Off');
const btn2On = document.getElementById('btn2On');
const btn2Off = document.getElementById('btn2Off');
const btn3On = document.getElementById('btn3On');
const btn3Off = document.getElementById('btn3Off');

// Database paths for GPIO states
const dbPathOutput1 = 'board1/outputs/digital/12';
const dbPathOutput2 = 'board1/outputs/digital/13';
const dbPathOutput3 = 'board1/outputs/digital/14';

// Database references using modular API
const dbRefOutput1 = ref(database, dbPathOutput1);
const dbRefOutput2 = ref(database, dbPathOutput2);
const dbRefOutput3 = ref(database, dbPathOutput3);

// Manage Login/Logout UI
const setupUI = (user) => {
  console.log('setupUI called with user:', user ? user.email : null);
  if (user) {
    // Toggle UI elements for logged-in state
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display = 'block';
    userDetailsElement.style.display = 'block';
    userDetailsElement.innerHTML = user.email;

    // Update states depending on database value
    onValue(dbRefOutput1, (snap) => {
      stateElement1.textContent = snap.val() === 1 ? "ON" : "OFF";
    });

    onValue(dbRefOutput2, (snap) => {
      stateElement2.textContent = snap.val() === 1 ? "ON" : "OFF";
    });

    onValue(dbRefOutput3, (snap) => {
      stateElement3.textContent = snap.val() === 1 ? "ON" : "OFF";
    });

    // Update database upon button click
    btn1On.addEventListener('click', () => set(dbRefOutput1, 1));
    btn1Off.addEventListener('click', () => set(dbRefOutput1, 0));
    btn2On.addEventListener('click', () => set(dbRefOutput2, 1));
    btn2Off.addEventListener('click', () => set(dbRefOutput2, 0));
    btn3On.addEventListener('click', () => set(dbRefOutput3, 1));
    btn3Off.addEventListener('click', () => set(dbRefOutput3, 0));
  } else {
    // Toggle UI elements for logged-out state
    loginElement.style.display = 'block';
    authBarElement.style.display = 'none';
    userDetailsElement.style.display = 'none';
    contentElement.style.display = 'none';
  }
};

// Export setupUI for use in other modules
export { setupUI };
