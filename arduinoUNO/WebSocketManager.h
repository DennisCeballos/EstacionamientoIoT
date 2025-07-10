#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecure.h>

#define USE_SERIAL Serial

class WebSocketManager {
public:
    static WebSocketManager* instance;

    WebSocketManager();      // Constructor
    ~WebSocketManager();     // Destructor

    void begin();            // New: called in Arduino setup()
    void loop();             // Called in Arduino loop()

    void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);

private:
    WiFiMulti WiFiMulti;
    WebSocketsClient webSocket;

    void hexdump(const void* mem, uint32_t len, uint8_t cols = 16);

    static void webSocketEventStatic(WStype_t type, uint8_t* payload, size_t length) {
        if (instance) {
            instance->webSocketEvent(type, payload, length);
        }
    }
};

#endif // WEBSOCKETMANAGER_H
