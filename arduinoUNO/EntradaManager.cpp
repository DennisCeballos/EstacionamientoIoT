/* --- EntradaManager.cpp --- */

#include "EntradaManager.h"
#include "componentes.h"
#include <Arduino.h>
#include <ESP32Servo.h>

EntradaManager::EntradaManager() = default;

EntradaManager::EntradaManager(int servoPin, Boton _boton, ReceptorIR _receptorIR, bool *_estadoEspacios, int _nroEspacios) : 
    botonEntrada(_boton), segmentoIR(_receptorIR), espaciosEstacionamiento(_estadoEspacios), nroEspacios(_nroEspacios)
{
    estadoAnterior = Estado::REPOSO;
    estado = Estado::REPOSO;

    // A este punto ya fueron inicializados los otros componentes

    // Inicializar el servomotor
    servoMotor.attach(servoPin);
    servoMotor.write(0);
}

void EntradaManager::ejecutarLoop()
{
    //Se obtienen datos de los sensores
    int valBoton = botonEntrada.isActive();
    int conteo = 0;
    Serial.print("Boton: ");
    Serial.println(valBoton);
    Serial.print("valIR: ");
    Serial.println(valIR);

    if (segmentoIR.leerSenal() < MINIMO_ESPACIO)
    {
        valIR = true;
    }
    else
    {
        valIR = false;
    }

    // Guardar el ultimo estado de ser distinto
    bool nuevoEstado = (estado != estadoAnterior);
    estadoAnterior = estado;

    // TODO this->debug_estados();

    // Comportamiento segun el estado que se encuentre

    Serial.print("DEBUG: Estado actual antes del switch: ");
    Serial.println(estado);

    switch (estado)
    {
    case Estado::REPOSO: // Estado relacionado al sistema en reposo
        Serial.println("EStoy en reposo");
        if (valBoton == true)
        {
            estado = Estado::CONSULTA;
        }
        break;

    case Estado::CONSULTA: // Estado relacionado a consultar si hay espacio disponible en el estacionamiento
    
        Serial.println("EStoy en CONSULTA");
        // Verificar cuantos espacios estan disponibles
        for (int i = 0; i < nroEspacios; i++)
        {
            // si es True, aumenta el conteo de espacios disponibles
            conteo = espaciosEstacionamiento[i] ? conteo + 1 : conteo;
        }

        Serial.println(conteo);

        // Si es que hay al menos un espacio disponible
        if (conteo > 0)
        {
            estado = Estado::ABIERTO;
        }
        else
        {
            estado = Estado::REPOSO;
        }
        break;

    case Estado::ABIERTO: // Estado relacionado a abrir y cerrar la puerta del estacionamiento
        Serial.println("EStoy en Abierto");

        //En caso sea la primera vez que se ingresa a este estado
        if (nuevoEstado)
        {
            //Abre la puerta con el servomotor
            servoMotor.write(180);

            //Toma el tiempo para saber cuanto tiempo esta abierto
            capturaTiempo = millis();
        }
        Serial.println(capturaTiempo);

        //Verifica que el tiempo que haya pasado con la puerta abierta sea menor al maximo tiempo definido
        if (millis() - capturaTiempo > MAX_TIEMPO_ABIERTO)
        {
            //Cierra la puerta
            servoMotor.write(0);
            estado = Estado::REPOSO;
        }
        else if (valIR == true) // Si es que se siente algo en el sensor
        {
            estado = Estado::SALIDA;
        }
        break;

    case Estado::SALIDA: // Estado relacionado con esperar a que el conductor pase la entrada
        Serial.println("EStoy en Salida");
        // Si es que ya no hay nadie en el sensor
        if (valIR == false)
        {
            //Cierra la puerta con el servomotor
            delay(1000);
            servoMotor.write(0);

            estado = Estado::REPOSO;
        }
        //si valIR es true entonces significa que aun hay alguien parado en la entrada
        break;

    default:
        Serial.println("LOL hubo un error ALGO grave con el automata");
        break;
    }

    Serial.print("estado: ");
    Serial.println(estado);
}

EntradaManager::~EntradaManager()
{
}