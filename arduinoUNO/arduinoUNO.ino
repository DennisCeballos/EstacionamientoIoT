/*
// Parking Manager
#include "componentes.h"

bool estados[8] = {true};

Multiplexor mux;
const int pinesMux[3] = {25, 32, 33};

Demultiplexor demux;
const int pinesDemux[3] = {21, 23, 22};

ShiftRegister shifReg;

SensorUltrasonico ultrasonico;

void setup()
{
  Serial.begin(115200);
  mux = Multiplexor(pinesMux, 26);
  demux = Demultiplexor(pinesDemux, 19);

  ultrasonico = SensorUltrasonico(19, 26);

  shifReg = ShiftRegister(16, 17, 5);
  shifReg.actualizarEstado(estados);
}

void loop()
{
  int val = 0;

  for (int i = 0; i <33; i++)
  {
    
    mux.seleccionarEntrada(i);
    demux.seleccionarSalida(i);

    Serial.print("Elegida la entrada ");
    Serial.println(i);

    valor = ultrasonico.medirDistancia();
    Serial.print("Distancia medida: ");
    Serial.println(valor);

    delay(5000);
    
    Serial.print("Se enciende el ");
    Serial.println(i);

    shifReg.escribir(i);

    delay(2000);
  }

}

*/


// Entrada Manager Compilable
#include "EntradaManager.h"
#include <ESP32Servo.h>

EntradaManager entrada;

bool estados[8] = {true};

Servo servomotor;

void setup()
{
Serial.begin(115200);

  /*
  entrada = EntradaManager(
      13,
      Boton(4),
      ReceptorIR(34),
      estados,
      8);
  */

  servomotor.attach(13);
  
}

void loop()
{
  for (int i = 0; i < 360; i=i+90)
  {
    servomotor.write(i);
  delay(2000);
  }
  //entrada.ejecutarLoop();
}




