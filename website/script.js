const wsUrl = window.location.protocol === 'https:' 
    ? `wss://${window.location.hostname}/ws`
    : `ws://${window.location.hostname}:81`;

if (window.location.protocol === 'file:') {
    wsUrl = 'ws://192.168.0.100:81';
}

console.log('Attempting to connect to:', wsUrl);
let ws = null;
let reconnectAttempts = 0;
const maxReconnectAttempts = 5;

function connectWebSocket() {
    ws = new WebSocket(wsUrl);

    ws.onopen = () => {
        console.log('Successfully connected to WebSocket');
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

    ws.onerror = (error) => {
        console.error('WebSocket Error:', error);
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