let map;
let heatmap;
let greenPotholeheatMapData = [];
let yellowPotholeheatMapData = [];
let amberPotholeheatMapData = [];
let redPotholeheatMapData = [];
let slippingHeatMapData = [];
let conjestionHeatMapData = [];

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

function showPotholeCheckboxes() {
  const checkboxesDiv = document.getElementById('checkboxes');
  checkboxesDiv.innerHTML = `
    <input type="checkbox" id="RedPotholes" onclick="updateHeatmap()"> Red Potholes
    <input type="checkbox" id="AmberPotholes" onclick="updateHeatmap()"> Amber Potholes
    <input type="checkbox" id="YellowPotholes" onclick="updateHeatmap()"> Yellow Potholes
    <input type="checkbox" id="GreenPotholes" onclick="updateHeatmap()"> Green Potholes
  `;
  potholeEntriesContainer.innerHTML = '';
  listenForPotholeChanges();
  highlightButton('potholeButton');

}

function showSlippingCheckboxes() {
  const checkboxesDiv = document.getElementById('checkboxes');
  checkboxesDiv.innerHTML = '';
  potholeEntriesContainer.innerHTML = '';
  listenForSlippingChanges();
  highlightButton('slippingButton');

}

function showCongestionCheckboxes() {
  const checkboxesDiv = document.getElementById('checkboxes');
  checkboxesDiv.innerHTML = '';
  potholeEntriesContainer.innerHTML = '';
  listenForCongestionChanges();
  highlightButton('congestionButton');

}

function highlightButton(buttonId) {
  const buttons = document.querySelectorAll('#buttons button');
  buttons.forEach(button => {
    if (button.id === buttonId) {
      button.classList.add('selected-button');
    } else {
      button.classList.remove('selected-button');
    }
  });
}


function listenForPotholeChanges() {
  const coordinatesRef = database.ref('potholes');

  coordinatesRef.on('child_added', (snapshot) => {
    const newPothole = snapshot.val();
    const potholeKey = snapshot.key; // Get the Firebase key

    try {
      // Access individual properties
      const latitude = newPothole.latitude;
      const longitude = newPothole.longitude;
      const weight = newPothole.weight;

      // Construct an object with the required structure
      const potholeData = {
        location: new google.maps.LatLng(latitude, longitude),
        // weight: weight
      };
      console.log("JSON String Parsed");
      const potholeEntryDiv = document.createElement('div');
      potholeEntryDiv.textContent = `Latitude: ${latitude}, Longitude: ${longitude}`;
      potholeEntriesContainer.appendChild(potholeEntryDiv);
      
      switch(weight) {
        case 0:
          greenPotholeheatMapData.push(potholeData);
          break;
        case 1:
          yellowPotholeheatMapData.push(potholeData);
          break;
        case 2:
          amberPotholeheatMapData.push(potholeData);
          break;
        case 3:
          redPotholeheatMapData.push(potholeData);
          break;
      }
      updateHeatmap();
    } catch (error) {
      // Handle the error gracefully
      console.error("Error parsing JSON:", error);
    }
  });
}

function listenForSlippingChanges() {
  const coordinatesRef = database.ref('slipping');

  coordinatesRef.on('child_added', (snapshot) => {
    const newSlipping = snapshot.val();
    const key = snapshot.key; // Get the Firebase key

    try {
      // Access individual properties
      const latitude = newSlipping.latitude;
      const longitude = newSlipping.longitude;

      // Construct an object with the required structure
      const slippingData = {
        location: new google.maps.LatLng(latitude, longitude),
        // weight: weight
      };
      console.log("JSON String Parsed");
      const potholeEntryDiv = document.createElement('div');
      potholeEntryDiv.textContent = `Latitude: ${latitude}, Longitude: ${longitude}`;
      potholeEntriesContainer.appendChild(potholeEntryDiv);
      
      slippingHeatMapData.push(slippingData);
      updateHeatmap();
    } catch (error) {
      // Handle the error gracefully
      console.error("Error parsing JSON:", error);
    }
  });
}

function listenForCongestionChanges() {
  const coordinatesRef = database.ref('congestion');

  coordinatesRef.on('child_added', (snapshot) => {
    const newCongestion = snapshot.val();
    const key = snapshot.key; // Get the Firebase key

    try {
      // Access individual properties
      const latitude = newCongestion.latitude;
      const longitude = newCongestion.longitude;
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

      conjestionHeatMapData.push(potholeData);
      updateHeatmap();
    } catch (error) {
      // Handle the error gracefully
      console.error("Error parsing JSON:", error);
    }
  });
}


function initMap() {
  map = new google.maps.Map(document.getElementById("map"), {
    zoom: 13,
    center: { lat: 50.445210, lng: -104.618896 },
  });

  // Initialize HeatmapLayer without data
  heatmap = new google.maps.visualization.HeatmapLayer({
    map: map,
  });

  // Call function to listen for coordinate changes
  console.log("Map Initialized");
}

function updateHeatmap() {
  try {
    const selectedDatasets = [];

    const gradientOptions = {
      red: ['rgba(255, 0, 0, 0)', 'rgba(255, 0, 0, 1)'],
      amber: ['rgba(255, 165, 0, 0)', 'rgba(255, 165, 0, 1)'],
      yellow: ['rgba(255, 255, 0, 0)', 'rgba(255, 255, 0, 1)'],
      green: ['rgba(0, 128, 0, 0)', 'rgba(0, 128, 0, 1)']
    };

    // Check if the pothole button is selected
    if (document.getElementById('potholeButton').classList.contains('selected-button')) {
      // Check if each checkbox is checked and add corresponding data to selectedDatasets
      if (document.getElementById('RedPotholes').checked) {
        selectedDatasets.push({
          data: redPotholeheatMapData,
          gradient: gradientOptions.red
        });
      }
      if (document.getElementById('AmberPotholes').checked) {
        selectedDatasets.push({
          data: amberPotholeheatMapData,
          gradient: gradientOptions.amber
        });
      }
      if (document.getElementById('YellowPotholes').checked) {
        selectedDatasets.push({
          data: yellowPotholeheatMapData,
          gradient: gradientOptions.yellow
        });
      }
      if (document.getElementById('GreenPotholes').checked) {
        selectedDatasets.push({
          data: greenPotholeheatMapData,
          gradient: gradientOptions.green
        });
      }
    }

    // Check if the slipping button is selected
    if (document.getElementById('slippingButton').classList.contains('selected-button')) {
      selectedDatasets.push({
        data: slippingHeatMapData,
      });
    }

    // Check if the congestion button is selected
    if (document.getElementById('congestionButton').classList.contains('selected-button')) {
      selectedDatasets.push({
        data: conjestionHeatMapData,
      });
    }

    // If no datasets are selected, clear the heatmap and return
    if (selectedDatasets.length === 0) {
      if (heatmap) {
        heatmap.setMap(null);
      }
      return;
    }

    // Combine all selected datasets
    let combinedData = [];
    selectedDatasets.forEach(dataset => {
      combinedData = combinedData.concat(dataset.data);
    });

    // Clear existing heatmap
    if (heatmap) {
      heatmap.setMap(null);
    }

    // Create new heatmap with customized gradients for each dataset
    heatmap = new google.maps.visualization.HeatmapLayer({
      data: combinedData,
      map: map,
      gradient: selectedDatasets.map(dataset => dataset.gradient).flat(),
    });
  } catch (error) {
    console.error("Error Updating Heatmap:", error);
  }
}



window.onload = function() {
  initMap();
  document.getElementById('potholeButton').click(); // Select the Pothole button
  document.getElementById('RedPotholes').checked = true;
  document.getElementById('AmberPotholes').checked = true;
  document.getElementById('YellowPotholes').checked = true;
  document.getElementById('GreenPotholes').checked = true;
  updateHeatmap(); // Update the heatmap accordingly

  
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
