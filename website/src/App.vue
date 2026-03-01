<template>
  <header>
      <div>
          <h1>Remote Monitoring Mode</h1>
          <p>
              The Flood Monitoring Device is currently in online!
          </p>
      </div>
  </header>
  <div class="list">
      <div class="container stack" id="number">
          <div class="tag">
              <h1>Water level: {{ waterLevel }}</h1>
              <p class="status" :class="{ active: connected }">
                {{  connected ? 'Connected (Keep-Alive Active)' : 'Reconnecting...' }}
              </p>
          </div>
          <div id="number_list" class="list">
              <h3>Last updated: {{ lastUpdated }}</h3>
          </div>
      </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue';

const url = 'https://untraveled-lingually-melda.ngrok-free.dev';

const waterLevel = ref('pending...');
const lastUpdated = ref('--');
const connected = ref(false);
const evenSource = ref(null);

// const fetchData = async () => {
//   try {
//     const response = await fetch(url+"/water-level", {
//       headers: {
//         'Accept': 'application/json',
//         'Connection': 'keep-alive',
//         'ngrok-skip-browser-warning': 'true'
//       }
//     });
//     const data = await response.json();
    
//     waterLevel.value = data.level + " cm";
//     lastUpdated.value = new Date(data.timestamp).toLocaleTimeString();
//     connected.value = true;
//   } catch (error) {
//     console.error("Polling error:", error);
//     connected.value = false;
//   }
// };

onMounted(() => {

  evenSource.value = new EventSource(url+"/water-level", {
    headers: {
      'Accept': 'text/event-stream',
      'Connection': 'keep-alive',
      'ngrok-skip-browser-warning': 'true'
    }
  });

  evenSource.value.onmessage = (event) => {
    const data = JSON.parse(event.data);
    waterLevel.value = data.level + " cm";
    lastUpdated.value = new Date(data.timestamp).toLocaleTimeString();
    connected.value = true;
  };

  evenSource.value.onerror = (error) => {
    console.error("SSE error:", error);
    connected.value = false;
  };
  
  // fetchData();
  // setInterval(fetchData, 2000);
});

onUnmounted(() => {
  if (evenSource.value) {
    evenSource.value.close();
  }
});
</script>