/**
 * Import function triggers from their respective submodules:
 *
 * const {onCall} = require("firebase-functions/v2/https");
 * const {onDocumentWritten} = require("firebase-functions/v2/firestore");
 *
 * See a full list of supported triggers at https://firebase.google.com/docs/functions
 */

// const {onRequest} = require("firebase-functions/v2/https");
// const logger = require("firebase-functions/logger");

// Create and deploy your first functions
// https://firebase.google.com/docs/functions/get-started

// exports.helloWorld = onRequest((request, response) => {
//   logger.info("Hello logs!", {structuredData: true});
//   response.send("Hello from Firebase!");
// });


const functions = require("firebase-functions");
const admin = require("firebase-admin");

admin.initializeApp({
    apiKey: "AIzaSyC05BlKrkXFGC3ZwBinu_PBuAmG6yemWMA",
    authDomain: "test-f1e70.firebaseapp.com",
    databaseURL: "https://test-f1e70.firebaseio.com",
    projectId: "test-f1e70",
    storageBucket: "test-f1e70.appspot.com",
    messagingSenderId: "539339171485",
    appId: "1:539339171485:web:a147d429439f5fbdfb5674",
  });

exports.rms_data = functions.https.onRequest((request, response) => {
    const uid = request.body.uid;
    const latitude = request.body.latitude;
    const longitude = request.body.longitude;
    const speed = request.body.speed;
    const messageType = request.body.message_type;

    // Generate timestamp on the server side
    const timestamp = new Date().toISOString();

    const data = {
        uid: uid,
        timestamp: timestamp,
        latitude: latitude,
        longitude: longitude,
        speed: speed,
        weight: messageType,
    };
    let location = null;
    // Save pothole data to the Firebase Realtime Database
    switch (messageType) {
        case 0:
        case 1:
        case 2:
        case 3:
            location = "potholes";
            break;
        case 4:
            location = "slipping";
            break;
        case 5:
            location = "conjection";
            break;
    }
    return admin.database().ref(location).push(data)
        .then(() => {
            console.log("Entry added successfully");
            response.status(200).send("Entry added successfully");
        })
        .catch((error) => {
            console.error("Error adding entry:", error);
            response.status(500).send("Error adding entry");
        });
});
