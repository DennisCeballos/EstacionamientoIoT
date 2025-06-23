const express = require('express');
const http = require('http');
const WebSocket = require('ws');

// Basic Express app (for Glitch to expose a web server)
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const ARDUINO_CLIENT = 'arduino';
const WEB_CLIENT = 'web';

const PARKING_STATE = {
  0: {
    0: 1,
    1: 1,
    2: 1,
    3: 1
  },
  1: {
    0: 1,
    1: 1,
    2: 1,
    3: 1
  }
};

// Optional: a basic home route
app.get('/', (req, res) => {
  // res.send('✅ WebSocket server is running.');
  res.sendFile('../client/public/index.html');
});

function syncAllWebclients(sender) {
  [...wss.clients].filter(client => client.type === WEB_CLIENT).forEach(webClient => {
    if (webClient.readyState === WebSocket.OPEN && webClient !== sender) {
      webClient.send(JSON.stringify(PARKING_STATE));
    }
  });
}

function syncArduinoClient() {
  const arduinoClient = [...wss.clients].find(client => client.type === ARDUINO_CLIENT);
  if (arduinoClient.readyState === WebSocket.OPEN) {
    // TODO: which message should the server send to arduino?
    let stateStr = '';
    for (let i = 0; i < 4; i++) {
      stateStr += PARKING_STATE[0][i] + ' ';
    }
    for (let i = 0; i < 3; i++) {
      stateStr += PARKING_STATE[1][i] + ' ';
    }
    stateStr += PARKING_STATE[1][3];
    console.log('Cadena de estado enviada a arduino: ', stateStr);
    arduinoClient.send(stateStr);
  }
}

function updateStateFromArduinoClient(stateStr) {
  const floor1 = stateStr.substring(0, 7).split(' ').map(val => Number(val));
  const floor2 = stateStr.substring(8).split(' ').map(val => Number(val));
  for (let i = 0; i < 4; i++) {
    PARKING_STATE[0][i] = PARKING_STATE[0][i] === 2 && floor1[i] === 1 ? PARKING_STATE[0][i] : floor1[i];
  }
  for (let i = 0; i < 4; i++) {
    PARKING_STATE[1][i] = PARKING_STATE[1][i] === 2 && floor2[i] === 1 ? PARKING_STATE[1][i] : floor2[i];
  }
  console.log('Estado actualizado desde arduino: ', stateStr);
  console.log(PARKING_STATE);
}

function updateStateFromWebClient(stateStr) {
  const [floor, space, status] = stateStr.split(' ').map(val => Number(val));
  PARKING_STATE[floor][space] = status;
  console.log('Estado actualizado desde web: ', stateStr);
  console.log(PARKING_STATE);
}

wss.on('connection', (wsClient, req) => {
  const clientType = req.headers['Client-Type'];
  console.log('👤 Se conecto un cliente: ' + (clientType || 'desconocido'));
  wsClient.type = clientType;

  wsClient.on('message', (message) => {
    console.log("📩 Mensaje recibido de un cliente: " + clientType);
    switch (clientType) {
      case ARDUINO_CLIENT:
        updateStateFromArduinoClient(message.toString());
        syncAllWebclients(wsClient);
        break;
      case WEB_CLIENT:
        updateStateFromWebClient(message.toString());
        syncAllWebclients(wsClient);
        syncArduinoClient();
        break;
      default:
        console.error('Un cliente desconodido envió un mensaje');
        break;
    }
  });

  wsClient.on('close', () => {
    console.log(`👋 Un cliente ${clientType} se deconecto`);
  });
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`🚀 Server listening on port ${PORT}`);
});
