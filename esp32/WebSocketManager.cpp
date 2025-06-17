#include "WebSocketManager.h"

#define WIFI_SSID "CASA"
#define WIFI_PASSWORD "12345678"
#define SERVIDOR_WEBSOCKET_DIRECCION "192.168.1.49"
#define SERVIDOR_WEBSOCKET_PUERTO 5000
#define SERVIDOR_WEBSOCKET_URL "/"

WebSocketManager::WebSocketManager() {
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(false);
    USE_SERIAL.println("\n\n[SETUP] Starting WebSocketManager...");

    for (uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        delay(1000);
    }

    wiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    WiFi.disconnect();
    while (wiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.begin(SERVIDOR_WEBSOCKET_DIRECCION, SERVIDOR_WEBSOCKET_PUERTO, SERVIDOR_WEBSOCKET_URL);
    webSocket.onEvent([this](WStype_t t, uint8_t* p, size_t l) { this->webSocketEvent(t, p, l); });
    webSocket.setReconnectInterval(5000);
    USE_SERIAL.println("[SETUP] WebSocketManager initialized.");
}

WebSocketManager::~WebSocketManager() {}

void WebSocketManager::loop() {
    webSocket.loop();
}

void WebSocketManager::hexdump(const void* mem, uint32_t len, uint8_t cols) {
    const uint8_t* src = (const uint8_t*)mem;
    USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
    for (uint32_t i = 0; i < len; i++) {
        if (i % cols == 0) {
            USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
        }
        USE_SERIAL.printf("%02X ", *src);
        src++;
    }
    USE_SERIAL.printf("\n");
}

void WebSocketManager::webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.println("[WSc] Disconnected!");
            break;

        case WStype_CONNECTED:
            USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
            webSocket.sendTXT("Connected");
            break;

        case WStype_TEXT:
            USE_SERIAL.printf("[WSc] Text received: %s\n", payload);
            webSocket.sendTXT("Hola pepe");
            break;

        case WStype_BIN:
            USE_SERIAL.printf("[WSc] Binary received, length: %u\n", length);
            hexdump(payload, length);
            break;

        default:
            break;
    }
}
