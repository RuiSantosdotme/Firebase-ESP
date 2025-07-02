import { initializeApp } from 'https://www.gstatic.com/firebasejs/10.14.1/firebase-app.js';
import { getAuth, signInWithEmailAndPassword } from 'https://www.gstatic.com/firebasejs/10.14.1/firebase-auth.js';
import { getStorage, ref, getDownloadURL, getMetadata } from 'https://www.gstatic.com/firebasejs/10.14.1/firebase-storage.js';

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

const email = 'AUTHORIZED_USER_EMAIL'; // Replace with your user email
const password = 'PASSWORD'; // Replace with your user password

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const storage = getStorage(app);

// Format date and time
const formatDateTime = (date) => {
  const pad = (num) => String(num).padStart(2, '0');
  return `${pad(date.getHours())}:${pad(date.getMinutes())}:${pad(date.getSeconds())} at ${date.getFullYear()}-${pad(date.getMonth() + 1)}-${pad(date.getDate())}`;
};

// Authenticate user
const authenticate = async () => {
  try {
    const userCredential = await signInWithEmailAndPassword(auth, email, password);
    return userCredential.user;
  } catch (error) {
    document.getElementById('date-time').textContent = `Authentication error: ${error.message}`;
    throw error;
  }
};

// Load image and metadata
const loadImageData = async (user) => {
  try {
    const storageRef = ref(storage, 'data/photo.jp');
    const url = await getDownloadURL(storageRef);
    document.querySelector('#img').src = url;

    const metadata = await getMetadata(storageRef);
    document.getElementById('date-time').textContent = formatDateTime(new Date(metadata.timeCreated));
  } catch (error) {
    document.getElementById('date-time').textContent = `Error: ${error.message}`;
  }
};

// Initialize on page load
document.addEventListener('DOMContentLoaded', async () => {
  const user = await authenticate();
  await loadImageData(user);

  document.querySelector('#refreshBtn').addEventListener('click', async () => {
    const user = await authenticate();
    await loadImageData(user);
  });
});
