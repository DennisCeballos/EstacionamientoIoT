#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h>

#define USE_SERIAL Serial1

class WebSocketManager {
public:
    WebSocketManager();      // Constructor
    ~WebSocketManager();     // Destructor

    void loop();             // Función pública de ejecución

private:
    WiFiMulti wiFiMulti;     // Declaración del objeto (sin lógica)
    WebSocketsClient webSocket;

    void hexdump(const void* mem, uint32_t len, uint8_t cols = 16);
    void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);
};

#endif // WEBSOCKETMANAGER_H
