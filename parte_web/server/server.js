const express = require('express');
const http = require('http');
const path = require('path');
const WebSocket = require('ws');

// Basic Express app (for Glitch to expose a web server)
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const ARDUINO_CLIENT = 'arduino';
const WEB_CLIENT = 'web';

// 0 = ocupado, 1 = disponible, 2 = reservado
const PARKING_STATE = {
  0: {
    0: 1,
    1: 1,
    2: 1
  },
  1: {
    0: 1,
    1: 1,
    2: 1,
    3: 1,
    4: 1
  }
};

function syncAllWebclients(sender) {
  [...wss.clients].filter(client => client.type === WEB_CLIENT).forEach(webClient => {
    if (webClient.readyState === WebSocket.OPEN && webClient !== sender) {
      webClient.send(JSON.stringify(PARKING_STATE));
    }
  });
}

function buildArduinoStateStr() {
  let stateStr = '';
  for (let i = 0; i < 3; i++) {
    stateStr += PARKING_STATE[0][i] + ' ';
  }
  for (let i = 0; i < 4; i++) {
    stateStr += PARKING_STATE[1][i] + ' ';
  }
  stateStr += PARKING_STATE[1][4];
  // let stateStr = '';
  // for (let i = 0; i < 4; i++) {
  //   stateStr += (PARKING_STATE[0][i] === 2 ? 1 : 0) + ' ';
  // }
  // for (let i = 0; i < 3; i++) {
  //   stateStr += (PARKING_STATE[1][i] === 2 ? 1 : 0) + ' ';
  // }
  // stateStr += PARKING_STATE[1][3] === 2 ? 1 : 0;
  return stateStr;
}

function syncAllArduinoClients() {
  // para el cliente arduino y la pagina config que simula ser arduino
  const stateStr = buildArduinoStateStr();
  [...wss.clients].filter(client => client.type === ARDUINO_CLIENT).forEach(arduinoClient => {
    if (arduinoClient.readyState === WebSocket.OPEN) {
      arduinoClient.send(stateStr);
      console.log('Cadena de estado enviada a arduino: ', stateStr);
    }
  });
}

function updateStateFromArduinoClient(stateStr) {
  // Validate format: exactly 8 numbers (0 or 1) separated by spaces
  const formatRegex = /^(0|1) (0|1) (0|1) (0|1) (0|1) (0|1) (0|1) (0|1)$/;
  if (!formatRegex.test(stateStr.trim())) {
    console.log('Error de formato, recibido:', stateStr);
    return;
  }

  // Proceed with state update
  const values = stateStr.trim().split(' ').map(Number);
  const floor1 = values.slice(0, 3);
  const floor2 = values.slice(3);

  // si esta libre en el circuito y reservado en el back, el estado no cambia en el back
  for (let i = 0; i < 3; i++) {
    PARKING_STATE[0][i] = PARKING_STATE[0][i] === 2 && floor1[i] === 1 ? PARKING_STATE[0][i] : floor1[i];
  }
  for (let i = 0; i < 5; i++) {
    PARKING_STATE[1][i] = PARKING_STATE[1][i] === 2 && floor2[i] === 1 ? PARKING_STATE[1][i] : floor2[i];
  }

  console.log('Estado actualizado desde Arduino:', stateStr);
  console.log(PARKING_STATE);
}


function updateStateFromWebClient(stateStr) {
  const [floor, spot, status] = stateStr.split(' ').map(val => Number(val));
  PARKING_STATE[floor][spot] = status;
  console.log('Estado actualizado desde web: ', stateStr);
  console.log(PARKING_STATE);
}

wss.on('connection', (wsClient, req) => {
  const clientType = wsClient.protocol;
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
        syncAllArduinoClients();
        break;
      default:
        console.error('Un cliente desconodido envió un mensaje');
        break;
    }
  });

  wsClient.on('close', () => {
    console.log('👋 Se deconecto un cliente: ' + clientType);
  });

  wsClient.send(JSON.stringify(PARKING_STATE));
});

app.use(express.text());

app.get('/reservaciones', (req, res) => {
  res.contentType('txt');
  res.send(buildArduinoStateStr());
});

app.post('/actualizar-estado', (req, res) => {
  updateStateFromArduinoClient(req.body);
  syncAllWebclients();
  res.sendStatus(200);
});

app.use('/', express.static(path.join(__dirname, '..', 'client/dist')));
app.get('*', (_, res) => {
  res.sendFile(path.join(__dirname, '..', 'client/dist/index.html'));
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`🚀 Server listening on port ${PORT}`);
});
