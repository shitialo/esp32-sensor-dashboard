const wsUrl = 'ws://192.168.0.100:81';
let ws = null;
let reconnectAttempts = 0;
const maxReconnectAttempts = 5;

function connectWebSocket() {
    ws = new WebSocket(wsUrl);

    ws.onopen = () => {
        console.log('Connected to ESP32');
        document.getElementById('connection-status').textContent = 'Connected';
        document.getElementById('connection-status').classList.add('connected');
        reconnectAttempts = 0;
    };

    ws.onclose = () => {
        document.getElementById('connection-status').textContent = 'Disconnected';
        document.getElementById('connection-status').classList.remove('connected');
        
        if (reconnectAttempts < maxReconnectAttempts) {
            reconnectAttempts++;
            setTimeout(connectWebSocket, 5000);
        }
    };

    ws.onmessage = (event) => {
        try {
            const data = JSON.parse(event.data);
            document.getElementById('temperature').textContent = `${data.temperature.toFixed(1)}°C`;
            document.getElementById('humidity').textContent = `${data.humidity.toFixed(1)}%`;
        } catch (e) {
            console.error('Error parsing data:', e);
        }
    };
}

connectWebSocket(); 