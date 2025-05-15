import { auth } from "./index.js";
import { signInWithEmailAndPassword, signOut, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";

document.addEventListener("DOMContentLoaded", () => {
  // Listen for auth status changes
  onAuthStateChanged(auth, (user) => {
    if (user) {
      console.log("User logged in:", user.email);
      setupUI(user);
    } else {
      console.log("User logged out");
      setupUI(null);
    }
  });

  // Login
  const loginForm = document.querySelector('#login-form');
  loginForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const email = loginForm['input-email'].value;
    const password = loginForm['input-password'].value;
    try {
      await signInWithEmailAndPassword(auth, email, password);
      loginForm.reset();
      console.log("Logged in:", email);
    } catch (error) {
      document.getElementById("error-message").innerHTML = error.message;
      console.error("Login error:", error.message);
    }
  });

  // Logout
  const logoutLink = document.querySelector('#logout-link');
  logoutLink.addEventListener('click', async (e) => {
    e.preventDefault();
    try {
      await signOut(auth);
      console.log("User signed out");
    } catch (error) {
      console.error("Logout error:", error.message);
    }
  });
});
