/* --- componentes.h --- */
/*
Clase para definir todos los componentes para el proyecto
*/
#ifndef COMPONENTES_H
#define COMPONENTES_H
#include <Arduino.h>

// Estructura para controlar "un rayo" (un segmento) conformado por un emisor y un receptor IR
typedef struct ReceptorIR
{
    int pinReceptorIR;

    ReceptorIR() = default;

    ReceptorIR(int _pin): pinReceptorIR(_pin)
    {
        pinMode(pinReceptorIR, INPUT);
    }

    // hace una lectura sobre la cantidad de senal IR recibido por el receptor
    int leerSenal()
    {
        return analogRead(pinReceptorIR);
    }

} ReceptorIR;

// Estructura para controlar un boton
typedef struct Boton
{
    int pin;

    Boton() = default;

    Boton(int _pin): pin(_pin)
    {
        pinMode(pin, INPUT_PULLUP); //esto va hacer que siempre detecte HIGH la entrada (a menos que se le aplique una corriente negativa)
    }

    bool isActive()
    {
        return !digitalRead(pin);
    }

} Boton;

// Estructura para controlar un LED
typedef struct LED
{
    int pin;

    LED() = default;

    LED(int _pin) : pin(_pin)
    {
        pinMode(pin, OUTPUT);
    }

    void encender() const
    {
        digitalWrite(pin, HIGH);
    }

    void apagar() const
    {
        digitalWrite(pin, LOW);
    }

} LED;

// Estructura de un sensor ultrasonico
typedef struct SensorUltrasonico
{
    int triggerPin;
    int echoPin;

    SensorUltrasonico() = default;

    SensorUltrasonico(int _triggerPin, int _echoPin) : triggerPin(_triggerPin), echoPin(_echoPin)
    {
        // definir los pinmode del sensor
        pinMode(triggerPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }

    int medirDistancia() const
    {
        // limpiar sensor (?)
        digitalWrite(triggerPin, LOW);
        delayMicroseconds(2);
        // emitir onda ultrasonido
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPin, LOW);

        // medir con echoPIN la duracion
        int duracion = pulseIn(echoPin, HIGH);

        // medida de veolcidad del sonido (cm/micro s)
        const float velocidadSonido = 0.03446f;
        // medida de distancia en cm
        int distancia = duracion * velocidadSonido / 2;

        // ? rando maximo de un sensor ultrasonico 4 metros (opcional?)
        // if (distancia >= 180) {
        //   return -1;
        // }
        return distancia;
    }

} SensorUltrasonico;

// Estructura de un Multiplexor con 8 pines de entrada DIGITAL
typedef struct Multiplexor //Lees datos de entrada (In) con selectores (S0, S1, S2) y los bota en SIG o Y
{
    int controlPins[3]; // S0, S1, S2
    int signalPin;          // salida SIG (o tambien el OUTPUT)

    Multiplexor() = default;

    Multiplexor(const int _controlPins[3], int _signalPin) : signalPin(_signalPin)
    {
        // Guardar los pines de control al array interno de la clase
        for (int i = 0; i < 3; ++i) {
            controlPins[i] = _controlPins[i];  // Copiar cada valor
        }

        for (int i = 0; i < 3; ++i)
        {
            pinMode(controlPins[i], OUTPUT);
        }
        pinMode(signalPin, INPUT_PULLUP);
    }

    void seleccionarEntrada(int pos_entrada)
    {
        // Serial.println("Se selecciono la salida: ");
        for (int i = 0; i < 3; ++i)
        {
            // la operacion de pos_entrada >> 1 & 1 obtiene el valor en bits del numero segun su posicion i
            digitalWrite(controlPins[i], (pos_entrada >> i) & 1);
            
            // Serial.print(controlPins[i]);
            // Serial.print(" - ");
            // Serial.println((pos_entrada >> i) & 1);
        }
        // Serial.println(pos_entrada);
    }

    int leerEntrada()
    {
        return digitalRead(signalPin);
    }
} Multiplexor;

// Estructura de un Demultiplexor con 8 pines de salida (3 control pins) DIGITAL
typedef struct Demultiplexor //Envia entrada (D) mediante al selector a una de las 8 salidas (In)
{
    int controlPins[3]; // S0, S1, S2
    int inputPin;           // Pin de entrada para ser redirigido
    int enablePin = -1;     // probablemente necesario

    Demultiplexor() = default;
    
    Demultiplexor(const int _controlPins[3], int _inputPin) : inputPin(_inputPin)
    {
        // Guardar los pines de control al array interno de la clase
        for (int i = 0; i < 3; ++i) {
            controlPins[i] = _controlPins[i];  // Copiar cada valor
        }

        // Inicializar los pines
        for (int i = 0; i < 3; ++i)
        {
            pinMode(controlPins[i], OUTPUT);
        }
        pinMode(inputPin, OUTPUT); //Para que envie la senal
    }

    void seleccionarSalida(int pos_salida)
    {
        // Serial.println("Se selecciono la salida: ");
        for (int i = 0; i < 3; ++i)
        {
            // la operacion de pos_salida >> 1 & 1 y obtiene el valor en bits del numero segun su posicion i
            digitalWrite(controlPins[i], (pos_salida >> i) & 1);
            
            // Serial.print(controlPins[i]);
            // Serial.print(" - ");
            // Serial.println((pos_salida >> i) & 1);
        }
        // Serial.println(pos_salida);
    }

    void enviarASalida(int salida, int valor)
    {
        seleccionarSalida(salida);
        digitalWrite(inputPin, valor);
    }
} Demultiplexor;

// Estructura de un ShiftRegister con 8 pines de salida
struct ShiftRegister {
    int dataPin;
    int clockPin;
    int latchPin;
    int value = 0;

    ShiftRegister() = default;

    ShiftRegister(int data, int clock, int latch)
        : dataPin(data), clockPin(clock), latchPin(latch)
    {
        pinMode(dataPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(latchPin, OUTPUT);    
    }

    // Envia el valor val en binario sobre la salida (val debe ser el entero decimal)
    void escribir(int val) {
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, val);
        digitalWrite(latchPin, HIGH);
        /*
        Serial.print("Escribiendo: ");
        Serial.print(val);
        Serial.print(" sobre: Data>");
        Serial.print(dataPin);
        Serial.print(" - Clock> ");
        Serial.print(clockPin);
        Serial.print(" - Latch> ");
        Serial.println(latchPin);
        */
    }

    // Definir el estado de un bit en especifico en el ShiftRegister
    void setBit(int bitIndex, bool valor) {
        if (bitIndex > 7) return;
        if (valor)
            value |= (1 << bitIndex);
        else
            value &= ~(1 << bitIndex);
        escribir(value);
    }

    void limpiar() {
        escribir(0);
    }
};


#endif // COMPONENTES_H