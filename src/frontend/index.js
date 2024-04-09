// This example requires the Visualization library. Include the libraries=visualization
// parameter when you first load the API. For example:
// <script src="https://maps.googleapis.com/maps/api/js?key=YOUR_API_KEY&libraries=visualization">
// let map, heatmap;


const firebaseConfig = {
  apiKey: "AIzaSyC05BlKrkXFGC3ZwBinu_PBuAmG6yemWMA",
  authDomain: "test-f1e70.firebaseapp.com",
  databaseURL: "https://test-f1e70.firebaseio.com",
  projectId: "test-f1e70",
  storageBucket: "test-f1e70.appspot.com",
  messagingSenderId: "539339171485",
  appId: "1:539339171485:web:72271bbd371b395dfb5674"
};
firebase.initializeApp(firebaseConfig);

// Get a reference to the database service
const database = firebase.database();

// Function to retrieve coordinates from Firebase and save them to cache
// Define potholeEntriesContainer globally
const potholeEntriesContainer = document.getElementById('potholeEntries');

function getAndSaveCoordinates() {
  const coordinatesRef = database.ref('potholes');

  coordinatesRef.once('value', (snapshot) => {
    const potholes = snapshot.val(); 
    for (const key in potholes) {
      if (potholes.hasOwnProperty(key)) {
        const pothole = potholes[key];
        const potholeEntryDiv = document.createElement('div');
        potholeEntryDiv.textContent = JSON.stringify(pothole);
        potholeEntriesContainer.appendChild(potholeEntryDiv);
      }
    }
  });
}

function listenForCoordinateChanges() {
  const coordinatesRef = database.ref('potholes');

  coordinatesRef.on('child_added', (snapshot) => {
    const newPothole = snapshot.val();
    const potholeEntryDiv = document.createElement('div');
    potholeEntryDiv.textContent = JSON.stringify(newPothole);
    potholeEntriesContainer.appendChild(potholeEntryDiv);
  });
}

window.onload = function() {
  getAndSaveCoordinates();
  listenForCoordinateChanges();
};



// function initMap() {
//   map = new google.maps.Map(document.getElementById("map"), {
//     zoom: 13,
//     center: { lat: 37.775, lng: -122.434 },
//   });
//   heatmap = new google.maps.visualization.HeatmapLayer({
//     data: getPoints(),
//     map: map,
//   });
//   document
//     .getElementById("toggle-heatmap")
//     .addEventListener("click", toggleHeatmap);
//   document
//     .getElementById("change-gradient")
//     .addEventListener("click", changeGradient);
//   document
//     .getElementById("change-opacity")
//     .addEventListener("click", changeOpacity);
//   document
//     .getElementById("change-radius")
//     .addEventListener("click", changeRadius);
// }

// function toggleHeatmap() {
//   heatmap.setMap(heatmap.getMap() ? null : map);
// }

// function changeGradient() {
//   const gradient = [
//     "rgba(0, 255, 255, 0)",
//     "rgba(0, 255, 255, 1)",
//     "rgba(0, 191, 255, 1)",
//     "rgba(0, 127, 255, 1)",
//     "rgba(0, 63, 255, 1)",
//     "rgba(0, 0, 255, 1)",
//     "rgba(0, 0, 223, 1)",
//     "rgba(0, 0, 191, 1)",
//     "rgba(0, 0, 159, 1)",
//     "rgba(0, 0, 127, 1)",
//     "rgba(63, 0, 91, 1)",
//     "rgba(127, 0, 63, 1)",
//     "rgba(191, 0, 31, 1)",
//     "rgba(255, 0, 0, 1)",
//   ];

//   heatmap.set("gradient", heatmap.get("gradient") ? null : gradient);
// }

// function changeRadius() {
//   heatmap.set("radius", heatmap.get("radius") ? null : 20);
// }

// function changeOpacity() {
//   heatmap.set("opacity", heatmap.get("opacity") ? null : 0.2);
// }

// // Heatmap data: 500 Points
// function getPoints() {
//   return [
    
//   ];
// }

// window.initMap = initMap;

