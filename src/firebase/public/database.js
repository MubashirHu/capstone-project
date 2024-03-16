let map;
let heatmap;
let heatMapData = [];


    // Proceed with your IndexedDB-related code
const firebaseConfig = {
    apiKey: "AIzaSyC05BlKrkXFGC3ZwBinu_PBuAmG6yemWMA",
    authDomain: "test-f1e70.firebaseapp.com",
    databaseURL: "https://test-f1e70.firebaseio.com",
    projectId: "test-f1e70",
    storageBucket: "test-f1e70.appspot.com",
    messagingSenderId: "539339171485",
    appId: "1:539339171485:web:a147d429439f5fbdfb5674",
};
firebase.initializeApp(firebaseConfig);

// Get a reference to the database service
const database = firebase.database();

// Function to retrieve coordinates from Firebase and save them to cache
// Define potholeEntriesContainer globally
const potholeEntriesContainer = document.getElementById('potholeEntries');


function listenForCoordinateChanges() {
  const coordinatesRef = database.ref('potholes');

  coordinatesRef.on('child_added', (snapshot) => {
    const newPothole = snapshot.val();
    const potholeKey = snapshot.key; // Get the Firebase key

    try {
      // Access individual properties
      const latitude = newPothole.latitude;
      const longitude = newPothole.longitude;
      // const weight = newPothole.weight;

      // Construct an object with the required structure
      const potholeData = {
        location: new google.maps.LatLng(latitude, longitude),
        // weight: weight
      };
      console.log("JSON String Parsed");
      const potholeEntryDiv = document.createElement('div');
      potholeEntryDiv.textContent = `Latitude: ${latitude}, Longitude: ${longitude}`;
      potholeEntriesContainer.appendChild(potholeEntryDiv);
      
      heatMapData.push(potholeData);
      updateHeatmap(heatMapData)
    } catch (error) {
      // Handle the error gracefully
      console.error("Error parsing JSON:", error);
    }
  });
}


function initMap() {
  map = new google.maps.Map(document.getElementById("map"), {
    zoom: 13,
    center: { lat: 37.775, lng: -122.434 },
  });

  // Initialize HeatmapLayer without data
  heatmap = new google.maps.visualization.HeatmapLayer({
    map: map,
  });

  // Call function to listen for coordinate changes
  console.log("Map Initialized");
  listenForCoordinateChanges();
}

function updateHeatmap(data) {
  // Set new heatmap data
  heatmap.setData(data);
}

window.onload = function() {
  initMap();
  
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
// }

// window.initMap = initMap;
