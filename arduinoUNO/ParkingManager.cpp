/* --- ParkingManager.cpp --- */
#include "componentes.h"
#include "parkingManager.h"

#define DISTANCIA_ACEPTADA 20

ParkingManager::ParkingManager(int nroLugares, Demultiplexor _demuxTriggers, Multiplexor _muxEchos, ShiftRegister _shiftRegLEDs) : 
                               nroEspacios(nroLugares), demuxTriggers(_demuxTriggers), muxEchos(_muxEchos), shiftRegLEDs(_shiftRegLEDs), ultrasonico(demuxTriggers.inputPin, muxEchos.signalPin)
{
    // En este punto ya fueron inicializados todos los componentes en su respectiva clase
    
    // excepto el ultrasonico local
    // este objeto solo se crea con la finalidad de hacer mas legible el codigo
    // ,pero realmente no es un elemento fisico sino solo el componente logico que se usa

    // Generar arrays para la disponibilidad de los espacios estacionamiento
    disponibilidadEspacios = new bool[nroEspacios];
}

// Calcula la disponibilidad de cada espacio
int ParkingManager::getEstadoEspacio(int pos)
{
    // cambia los multiplexores al espacio elegido
    this->demuxTriggers.seleccionarSalida(pos);
    this->muxEchos.seleccionarEntrada(pos);
    
    // mide la distancia con el ultrasonico
    // retorna true de estar ocupado
    if (ultrasonico.medirDistancia() < DISTANCIA_ACEPTADA)
    {
        // si detecta distancia menor a la aceptada significa que hay algo
        return false;
    }
    return true;
}

bool *ParkingManager::getEstado_Estacionamiento()
{
    // Recorrer todos los espacios de estacionamiento y registrar su disponibilidad
    for (int i = 0; i < this->nroEspacios; i++)
    {
        disponibilidadEspacios[i] = getEstadoEspacio(i);
    }

    // Encender los LEDs respectivos segun disponibilidad
    int valor = 0;
    // convierte el array de bool en un numero decimal aceptable por 
    for (int i = 0; i < 8; ++i) {
        if (disponibilidadEspacios[i]) {
            valor |= (1 << (7 - i)); //obtiene el valor decimal que forman los espacios si se convierte en binario
        }
    }
    
    // escribir los valores de disponibilidad en los LEDs
    shiftRegLEDs.escribir(valor);
    
    return disponibilidadEspacios;
}

bool ParkingManager::reservarEspacioEstacionamiento(int)
{
    return false;
}

ParkingManager::~ParkingManager() {}
