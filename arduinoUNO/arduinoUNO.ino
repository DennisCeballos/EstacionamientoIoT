// Parking Manager
#include "componentes.h"

bool estados[8] = {true};

Multiplexor mux;
const int pinesMux[3] = {33, 32, 25};

Demultiplexor demux;
const int pinesDemux[3] = {21, 23, 22};

SensorUltrasonico ultrasonico;

void setup()
{
  mux = Multiplexor(pinesMux, 26);
  demux = Demultiplexor(pinesDemux, 19);

  ultrasonico = SensorUltrasonico(19, 26);
}

void loop()
{
  int valor = 0;
  for (int i = 0; i < 8; i++)
  {
    mux.seleccionarEntrada(i);
    demux.seleccionarSalida(i);

    Serial.print("Elegida la entrada ");
    Serial.println(i);

    valor = ultrasonico.medirDistancia();
    Serial.print("Distancia medida: ");
    Serial.println(valor);
  }
}

/*

// Entrada Manager Compilable
#include "EntradaManager.h"

EntradaManager entrada;

bool estados[8] = {true};

void setup()
{
  entrada = EntradaManager(
      4,
      Boton(34),
      ReceptorIR(35),
      estados,
      8);
}

void loop()
{
  entrada.ejecutarLoop();
}





*/