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
      <div class="chart-wrapper">
        <Line :data="chartData" :options="chartOptions" />
      </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue';
import { fetchEventSource } from '@microsoft/fetch-event-source';

import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
} from 'chart.js'
import { Line } from 'vue-chartjs'

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
)

const url = 'https://untraveled-lingually-melda.ngrok-free.dev';

const waterLevel = ref('pending...');
const lastUpdated = ref('--');
const connected = ref(false);

const waterLevelHistory = ref([]);
const timeHistory = ref([]);

const chartData = ref({
  labels: timeHistory.value,
  datasets: [
    {
      label: 'Water Level (cm)',
      backgroundColor: '#54bc69',
      borderColor: '#54bc69',
      data: waterLevelHistory.value,
      fill: false
    }
  ]
});

const chartOptions = {
  responsive: true,
  maintainAspectRatio: false,
  animation: {
    duration: 400,     // The time (in milliseconds) the slide takes. 
    easing: 'linear',  // 'linear' gives it that smooth, mechanical slide rather than a snappy bounce.
  },

  plugins: {
    legend: {
      display: false // Hides the "Water Level (cm)" label at the top
    },
  },
  
  scales: {
    x: {
    },
    y: {
      beginAtZero: true
    }
  }
};

onMounted(() => {
  fetchEventSource(url+"/water-level", {
    headers: {
      'Accept': 'text/event-stream',
      'Connection': 'keep-alive',
      'ngrok-skip-browser-warning': 'true'
    },
    onmessage: (event) => {
      const data = JSON.parse(event.data);
      waterLevel.value = data.level + " cm";
      lastUpdated.value = new Date(data.timestamp).toLocaleTimeString();
      connected.value = true;

      const newData = [...chartData.value.datasets[0].data, data.level];
      
      const newLabels = newData.map((_, index) => {
        // 'x' is how many positions away the item is from the newest data point
        const x = newData.length - 1 - index;
        return x === 0 ? 'Now' : `${2 * x} sec ago`; 
      });
      if (newLabels.length > 10) {
        newLabels.shift();
        newData.shift();
      }

      chartData.value = {
        labels: newLabels,
        datasets: [
          {
            ...chartData.value.datasets[0],
            data: newData
          }
        ]
      };
    },
    onerror: (error) => {
      console.error("SSE error:", error);
      connected.value = false;
    }
  });
});
</script>