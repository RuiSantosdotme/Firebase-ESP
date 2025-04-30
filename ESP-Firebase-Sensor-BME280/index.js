import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-app.js";
import { getAuth } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";
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
const auth = getAuth(app);
const database = getDatabase(app);

// Export auth for use in auth.js
export { auth };

// UI Elements
const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");
const tempElement = document.getElementById("temp");
const humElement = document.getElementById("hum");
const presElement = document.getElementById("pres");

// Manage Login/Logout UI
const setupUI = (user) => {
  if (user) {
    // Toggle UI elements
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display = 'block';
    userDetailsElement.style.display = 'block';
    userDetailsElement.innerHTML = user.email;

    // Database paths
    const uid = user.uid;
    const dbPathTemp = `UsersData/${uid}/temperature`;
    const dbPathHum = `UsersData/${uid}/humidity`;
    const dbPathPres = `UsersData/${uid}/pressure`;

    // Database references
    const dbRefTemp = ref(database, dbPathTemp);
    const dbRefHum = ref(database, dbPathHum);
    const dbRefPres = ref(database, dbPathPres);

    // Update page with new readings
    onValue(dbRefTemp, (snap) => {
      tempElement.innerText = snap.val()?.toFixed(2) ?? "N/A";
    });

    onValue(dbRefHum, (snap) => {
      humElement.innerText = snap.val()?.toFixed(2) ?? "N/A";
    });

    onValue(dbRefPres, (snap) => {
      presElement.innerText = snap.val()?.toFixed(2) ?? "N/A";
    });
  } else {
    // Toggle UI elements
    loginElement.style.display = 'block';
    authBarElement.style.display = 'none';
    userDetailsElement.style.display = 'none';
    contentElement.style.display = 'none';
  }
};

// Expose setupUI to global scope for auth.js
window.setupUI = setupUI;
