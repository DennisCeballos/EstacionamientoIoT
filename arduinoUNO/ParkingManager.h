/* --- ParkingManager.h --- */

#pragma once

#ifndef PARKINGMANAGER_H
#define PARKINGMANAGER_H

// Clase que gestiona todas las interacciones con los parking spots
/*
la clase ASUME que cada lugar de estacionamiento cuenta con un sensor ultrasonico y un LED
Asi como asume que hay:
    un demultiplexor para los triggers ultrasonicos
    un multiplexor para los echo ultrasonicos
    un shiftRegister para los LEDs
*/
class ParkingManager
{
public:
    ParkingManager(int nroLugares, Demultiplexor _demuxTriggers, Multiplexor _muxEchos, ShiftRegister _shiftRegLEDs);

    int getEstadoEspacio(int);

    bool *getEstado_Estacionamiento();

    bool reservarEspacioEstacionamiento(int);

    ~ParkingManager();

private:
    int nroEspacios;
    Demultiplexor demuxTriggers;
    Multiplexor muxEchos;
    ShiftRegister shiftRegLEDs;
    SensorUltrasonico ultrasonico;

    bool *disponibilidadEspacios;
};

#endif // PARKINGMANAGER_H
