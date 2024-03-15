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
    // getAndSaveCoordinates();
    listenForCoordinateChanges();
  };
  
  