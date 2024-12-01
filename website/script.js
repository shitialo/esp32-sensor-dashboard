const getWebSocketUrl = () => {
    if (window.location.protocol === 'file:') {
        return 'ws://192.168.0.100:81';
    }
    
    const wsProtocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    return `${wsProtocol}//${window.location.host}/ws`;
}

const wsUrl = getWebSocketUrl();
console.log('Connecting to WebSocket at:', wsUrl);
let ws = null;
let reconnectAttempts = 0;
const maxReconnectAttempts = 5;

function connectWebSocket() {
    try {
        ws = new WebSocket(wsUrl);

        ws.onopen = () => {
            console.log('Successfully connected to WebSocket');
            document.getElementById('connection-status').textContent = 'Connected';
            document.getElementById('connection-status').classList.add('connected');
            reconnectAttempts = 0;
        };

        ws.onclose = () => {
            console.log('WebSocket connection closed');
            document.getElementById('connection-status').textContent = 'Disconnected';
            document.getElementById('connection-status').classList.remove('connected');
            
            if (reconnectAttempts < maxReconnectAttempts) {
                reconnectAttempts++;
                console.log(`Reconnecting... Attempt ${reconnectAttempts}`);
                setTimeout(connectWebSocket, 5000);
            }
        };

        ws.onerror = (error) => {
            console.error('WebSocket Error:', error);
            console.log('Current WebSocket URL:', wsUrl);
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
    } catch (error) {
        console.error('Error creating WebSocket:', error);
    }
}

connectWebSocket(); 