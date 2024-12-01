const express = require('express');
const { createProxyMiddleware } = require('http-proxy-middleware');
const app = express();

// Serve static files
app.use(express.static('website'));

// Proxy WebSocket connections
app.use('/ws', createProxyMiddleware({ 
    target: 'ws://102.219.208.124:81',
    ws: true,
    changeOrigin: true,
    secure: false,
    onError: (err, req, res) => {
        console.error('Proxy Error:', err);
    }
}));

const port = process.env.PORT || 3000;
app.listen(port, () => {
    console.log(`Server running on port ${port}`);
}); 