/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */
#include "EntradaManager.h"
#include "componentes.h"

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsClient.h>

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial

char output_web[17]; // char array para mantener la rpta que se envia al webSocket

void boolArrayToCharArray(bool estados[8], char output[17])
{
  // 8 bool values, plus 7 spaces, and null terminator = 17 chars in total
  int index = 0;

  for (int i = 0; i < 8; ++i)
  {
    // Convert the bool value to '0' or '1'
    output[index++] = (estados[i] ? '1' : '0');

    // Add a space between each character, but not after the last one
    if (i < 7)
    {
      output[index++] = ' ';
    }
  }

  // Null-terminate the string
  output[index] = '\0';
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
  const uint8_t *src = (const uint8_t *)mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++)
  {
    if (i % cols == 0)
    {
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

    // send message to server when Connected
    webSocket.sendTXT("Connected");
    break;
  case WStype_TEXT:
    USE_SERIAL.printf("[WSc] get text: %s\n", payload);

    // send message to server
    webSocket.sendTXT("Hola pepe");
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);

    // send data to server
    // webSocket.sendBIN(payload, length);
    break;
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

bool estados[8] = {true};
bool reservado[8] = {false};

Multiplexor mux;
const int pinesMux[3] = {25, 32, 33}; // orden de los pines del chip: 11, 10, 9

Demultiplexor demux;
const int pinesDemux[3] = {21, 23, 22}; // orden de los pines del chip: 1, 2, 3
SensorUltrasonico ultrasonico;

EntradaManager entrada;

ShiftRegister shifReg;

void setup()
{
  // USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);
  USE_SERIAL.println("Esta pasando algo?");

  // Serial.setDebugOutput(true);
  USE_SERIAL.setDebugOutput(false);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println("Si");

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  USE_SERIAL.println("Parece ser");

  WiFiMulti.addAP("evie", "eviephone$1");

  WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    USE_SERIAL.println("Haciendo algo");
    delay(100);
  }
  USE_SERIAL.println("creo");

  // server address, port and URL
  webSocket.begin("192.168.108.33", 3000, "/");
  USE_SERIAL.println("empezamos");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  // webSocket.setAuthorization("user", "Password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  USE_SERIAL.println("Se acabo el setup");

  mux = Multiplexor(pinesMux, 26);
  demux = Demultiplexor(pinesDemux, 19);

  ultrasonico = SensorUltrasonico(19, 26);

  shifReg = ShiftRegister(16, 17, 5); // orden de los pines del chip: 14, 11, 12
  shifReg.actualizarEstado(estados);

  entrada = EntradaManager(
      13,
      Boton(4),
      ReceptorIR(34),
      estados,
      8);
}

void loop()
{

  int valor = 0;

  for (int i = 0; i < 5; i++)
  {

    if (reservado[i])
    {
      continue;
    }

    mux.seleccionarEntrada(i);
    demux.seleccionarSalida(i);

    Serial.print("Elegida la entrada ");
    Serial.println(i);

    valor = ultrasonico.medirDistancia();
    Serial.print("Distancia medida: ");
    Serial.println(valor);

    if (valor < 5)
    {
      estados[i] = false;
      Serial.println("OCUPAO!");
    }
    else
    {
      estados[i] = true;
      Serial.println("> LIBRE");
    }

    // Serial.print("Se enciende el ");
    // Serial.println(i);
    // shifReg.escribir(i);
  }

  shifReg.actualizarEstado(estados);
  entrada.ejecutarLoop();

  webSocket.loop();

  boolArrayToCharArray(estados, output_web);
  Serial.println(output_web); // Deberia imprimir: "1 0 1 0 1 0 1 0"
  webSocket.sendTXT(output_web);

  delay(20);
}

/*
COPIA FUNCIONAL DE NADA WEB FUNCIONANDO
// Entrada Manager Compilable
#include "EntradaManager.h"
#include <ESP32Servo.h>
#include "componentes.h"

bool estados[8] = {true};

Multiplexor mux;
const int pinesMux[3] = {25, 32, 33};

Demultiplexor demux;
const int pinesDemux[3] = {21, 23, 22};
SensorUltrasonico ultrasonico;

EntradaManager entrada;

ShiftRegister shifReg;

void setup()
{
  Serial.begin(115200);
  mux = Multiplexor(pinesMux, 26);
  demux = Demultiplexor(pinesDemux, 19);

  ultrasonico = SensorUltrasonico(19, 26);

  shifReg = ShiftRegister(16, 17, 5);
  shifReg.actualizarEstado(estados);

  entrada = EntradaManager(
    13,
    Boton(4),
    ReceptorIR(34),
    estados,
    8);
  }

  void loop()
  {
    int valor = 0;

    for (int i = 0; i < 5; i++)
    {

    mux.seleccionarEntrada(i);
    demux.seleccionarSalida(i);

    Serial.print("Elegida la entrada ");
    Serial.println(i);

    valor = ultrasonico.medirDistancia();
    Serial.print("Distancia medida: ");
    Serial.println(valor);

    if (valor < 2)
    {
      estados[i] = false;
    }


    //Serial.print("Se enciende el ");
    //Serial.println(i);
    //shifReg.escribir(i);

}

shifReg.actualizarEstado(estados);
entrada.ejecutarLoop();

delay(2000);
}
*/