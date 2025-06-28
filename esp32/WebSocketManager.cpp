#include "WebSocketManager.h"

#define WIFI_SSID "Galaxy S24 FF70"
#define WIFI_PASSWORD "Kserola2830"
#define SERVIDOR_WEBSOCKET_DIRECCION "192.168.159.164"
#define SERVIDOR_WEBSOCKET_PUERTO 3000
#define SERVIDOR_WEBSOCKET_URL "/"

WebSocketManager* WebSocketManager::instance = nullptr;

// Minimal constructor (only stores instance pointer)
WebSocketManager::WebSocketManager() {
    instance = this;
}

WebSocketManager::~WebSocketManager() {}

void WebSocketManager::begin() {
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(true);
    USE_SERIAL.println("\n\n[SETUP] Starting WebSocketManager...");

    for (uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        delay(1000);
    }

    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    USE_SERIAL.println("Conectando a la WiFi...");

    WiFi.disconnect();
    while (WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    USE_SERIAL.println("WiFi conectada.");

    webSocket.begin(SERVIDOR_WEBSOCKET_DIRECCION, SERVIDOR_WEBSOCKET_PUERTO, SERVIDOR_WEBSOCKET_URL);
    webSocket.onEvent(WebSocketManager::webSocketEventStatic);
    webSocket.setReconnectInterval(5000);

    USE_SERIAL.println("[SETUP] WebSocketManager inicializado.");
}

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
