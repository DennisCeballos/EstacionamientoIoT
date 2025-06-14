#include <Servo.h>

/*
 * Configurar Pines
 */
#define NRO_ULTRASONICOS 4
bool coleccion_Ultrasonicos[NRO_ULTRASONICOS] = {};
// Pines de Multiplexor Echo
/* Requiere: 4 digital
 */
const int muxEcho_S0 = 4;
const int muxEcho_S1 = 5;
const int muxEcho_S2 = 6;
const int muxEcho_Z = 7; 

// Pines de Multiplexor Trigger
const int muxTrigger_S0 = 10;
const int muxTrigger_S1 = 11;
const int muxTrigger_S2 = 12;
const int muxTrigger_Z = 13; 

// Pines de leds
const int shiftRegLed_S0 = 1;
const int shiftRegLed_S1 = 2;
const int shiftRegLed_S2 = 8;
const int shiftRegLed_Z = 8; 

// --- Elementos relacionados a la entrada ---
enum Estado
{
  REPOSO,
  CONSULTA,
  ABIERTO,
  SALIDA
} estado = REPOSO;
enum Estado estadoAnterior;
unsigned long capturaTiempo;

// Pines de input Boton
/* Requiere: 1 input digital
 */
int entradaBoton = 3, valBoton = 0;
long t = 0, d = 0;

// Pines de input sensor IR
/* Requiere: 1 input analogico
 */
#define MINIMO_ESPACIO 100 // minimo valor para determinar si hay algo entre ambos sensores o no
int entradaIR = A0;        // el sensor IR retorna un valor analogico, pero para el proyecto es necesario manejarlo como 1 o 0
bool valIR = false;        // TRUE = hay algo; FALSE = no hay nada

// Pines de servo motor
/* Requiere: 1 digital
 */
int servoPin = 9;
Servo servoMotor;
#define MAX_TIEMPO_ABIERTO 3000 // define el tiempo maximo para que la puerte este abierta en milisegundos

// Setup de variables
void setup()
{
  // put your setup code here, to run once:
  servoMotor.attach(servoPin);
  pinMode(entradaIR, INPUT);
  pinMode(entradaBoton, INPUT);

  // Mux Echo
  pinMode(muxEcho_S0, OUTPUT);
  pinMode(muxEcho_S1, OUTPUT);
  pinMode(muxEcho_S2, OUTPUT);
  pinMode(muxEcho_Z, OUTPUT);

  // Mux Trigger
  pinMode(muxTrigger_S0, OUTPUT);
  pinMode(muxTrigger_S1, OUTPUT);
  pinMode(muxTrigger_S2, OUTPUT);
  pinMode(muxTrigger_Z, OUTPUT);

  Serial.begin(9600);
}

void selectChannel(int s0, int s1, int s2, int channel)
{
  digitalWrite(s0, bitRead(channel, 0));
  digitalWrite(s1, bitRead(channel, 1));
  digitalWrite(s2, bitRead(channel, 2));
}

void loop()
{
  //
  // INSPECCIONAR TODOS LOS SENSORES ULTRASONICOS
  //
  // Recorrer la lista de multiplexores
  // Por cada elemento del multiplexor, medir la distancia

  for (int i = 0; i < NRO_ULTRASONICOS; i++)
  {
    // Seleccionar el canal del multiplexor para escribir
    selectChannel(muxTrigger_S0, muxTrigger_S1, muxTrigger_S2, i);
    // Escribir el valor del multiplexor
    digitalWrite(muxTrigger_Z, HIGH);
    delay(10);
    digitalWrite(muxTrigger_Z, LOW);

    // Seleccionar el canal del multiplexor para leer
    selectChannel(muxEcho_S0, muxEcho_S1, muxEcho_S2, i);

    // Leer el valor del multiplexor
    t = pulseIn(muxEcho_Z, HIGH);
    d = t/59;
    
    Serial.print("Distancia: ");
    Serial.print(d);


    // Guardar el valor en la lista de ultrasonicos
    // Actualizar la lista del "ESTADO"(Ocupado, Desocupado) de los slots

    if (d > MINIMO_ESPACIO)
    {
      coleccion_Ultrasonicos[i] = true;
      delay(2000);
    }
    else
    {
      coleccion_Ultrasonicos[i] = false;
    }
  }

  //
  // SUBIR INFORMACIÓN AL SERVIDOR
  //
  // Enviar al servidor la lista del "ESTADO" de los slots

  //
  // Actualizar informacion en el LCD
  //

  //
  // VERIFICAR SI ALGUIEN "TOCA A LA PUERTA"
  // (IMPLEMENTACION DE AUTOMATA DE ESTADOS FINITOS)

  // Se obtienen datos de los sensores
  valBoton = digitalRead(entradaBoton);
  if (analogRead(entradaIR) < MINIMO_ESPACIO)
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

  debug_estados();

  // Comportamiento segun el estado que se encuentre
  switch (estado)
  {
  case Estado::REPOSO: // Estado relacionado al sistema en reposo
    if (valBoton == true)
    {
      estado = Estado::CONSULTA;
    }
    break;

  case Estado::CONSULTA: // Estado relacionado a consultar si hay espacio disponible en el estacionamiento
  {
    // Verificar cuantos ultrasonicos estan disponibles
    int conteo = 0;
    for (int i = 0; i < NRO_ULTRASONICOS; i++)
    {
      conteo = coleccion_Ultrasonicos[i] ? conteo + 1 : conteo;
      delay(500);
    }
    if (conteo < NRO_ULTRASONICOS)
    {
      estado = Estado::ABIERTO;
    }
    else
    {
      estado = Estado::REPOSO;
    }
  }
  break;

  case Estado::ABIERTO: // Estado relacionado a abrir y cerrar la puerta del estacionamiento

    Serial.println(nuevoEstado);

    // En caso sea la primera vez que se ingresa a este estado
    if (nuevoEstado)
    {
      // Abre la puerta con el servomotor
      servoMotor.write(180);
      Serial.println("Se toma el tiempo");

      // Toma el tiempo para saber cuanto tiempo esta abierto
      capturaTiempo = millis();
    }

    // Verifica que el tiempo que haya pasado con la puerta abierta sea menor al maximo tiempo definido
    if (millis() - capturaTiempo > MAX_TIEMPO_ABIERTO)
    {
      // Cierra la puerta
      servoMotor.write(0);
      estado = Estado::REPOSO;
    } else if (valIR == true)  // Si es que se siente algo en el sensor
    {
      estado = Estado::SALIDA;
    }
    break;

  case Estado::SALIDA: // Estado relacionado con esperar a que el conductor pase la entrada
    // Si es que ya no hay nadie en el sensor
    if (valIR == false)
    {
      // Cierra la puerta con el servomotor
      delay(1000);
      servoMotor.write(0);

      estado = Estado::REPOSO;
    }
    break;

  default:
    Serial.println("LOL hubo un error ALGO grave");
    break;
  }

  delay(1000);
}

inline void debug_estados()
{
  switch (estado)
  {
  case Estado::REPOSO:
    Serial.println("Estado::REPOSO");
    break;
  case Estado::CONSULTA:
    Serial.println("Estado::CONSULTA");
    break;
  case Estado::ABIERTO:
    Serial.println("Estado::ABIERTO");
    break;
  case Estado::SALIDA:
    Serial.println("Estado::SALIDA");
    break;

  default:
    break;
  }

  Serial.print("Anterior > ");
  switch (estadoAnterior)
  {
  case Estado::REPOSO:
    Serial.println("Estado::REPOSO");
    break;
  case Estado::CONSULTA:
    Serial.println("Estado::CONSULTA");
    break;
  case Estado::ABIERTO:
    Serial.println("Estado::ABIERTO");
    break;
  case Estado::SALIDA:
    Serial.println("Estado::SALIDA");
    break;

  default:
    break;
  }
  Serial.println("");
};
