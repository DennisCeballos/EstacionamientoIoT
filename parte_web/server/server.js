const express = require('express');
const http = require('http');
const WebSocket = require('ws');

// Basic Express app (for Glitch to expose a web server)
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Optional: a basic home route
app.get('/', (req, res) => {
  res.send('✅ WebSocket server is running.');
});

function broadcastToAll(message, sender = null) {
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN && client !== sender) {
      client.send(message);
    }
  });
}

wss.on('connection', (ws) => {
  console.log('👤 New client connected');
  ws.send('✅ Connection established with server');

  ws.on('message', (message) => {
    const msgStr = message.toString();
    console.log(`📩 Message received: "${msgStr}"`);

    if (msgStr.includes("ENVIANDO ALERTA")) {
      console.log('🚨 ALERT received! Broadcasting "FIREWORKS"...');
      broadcastToAll("FIREWORKS",wss);
    }
  });

  ws.on('close', () => {
    console.log('👋 A client has disconnected');
  });
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`🚀 Server listening on port ${PORT}`);
});
