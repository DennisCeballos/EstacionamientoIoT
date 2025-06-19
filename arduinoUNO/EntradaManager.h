/* --- EntradaManager.h --- */

#pragma once

#include "componentes.h"
#include <Servo.h>

#ifndef ENTRADAMANAGER_H
#define ENTRADAMANAGER_H

#define MAX_TIEMPO_ABIERTO 3000 // define el tiempo maximo para que la puerte este abierta en milisegundos
#define MINIMO_ESPACIO 100 // minimo valor para determinar si hay algo entre ambos sensores o no

// Definicion de estados para la logica del automata
typedef enum Estado
{
    REPOSO,
    CONSULTA,
    ABIERTO,
    SALIDA
} Estado;

// Clase para gestionar la entrada del estacionamiento
// implementa la logica de un automata finito para gestionar la entrada del estacionamiento
// asume que el circuito integra el uso de un servomotor, un boton regular y un receptor IR
// la clase requiere que se le assigne la referencia a un array de bool, que representa la disponibilidad de los estacionamientos
class EntradaManager
{
private:
    Servo servoMotor;
    Boton botonEntrada;
    ReceptorIR segmentoIR;
    Estado estadoAnterior;
    Estado estado;
    bool valIR;
    bool* espaciosEstacionamiento;
    int nroEspacios;
    unsigned long capturaTiempo;

public:

    EntradaManager();

    EntradaManager(int, Boton, ReceptorIR, bool*, int);

    void ejecutarLoop();

    ~EntradaManager();
};

#endif // ENTRADAMANAGER_H