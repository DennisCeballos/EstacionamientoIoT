#include<Servo.h>

/*
 * Configurar Pines
*/

#define NRO_ULTRASONICOS 4
bool coleccion_Ultrasonicos[NRO_ULTRASONICOS] = {};
// Pines de Multiplexor Analogico
/* Requiere: 4 digital
 */

// Pines de Multiplexor Digital
/* Requiere:
 */

// Pines de "solo un" sensor ultrasonico
/* Requiere: 2 analogicos
 */

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
int entradaBoton = 3;
int valBoton = 0;

// Pines de input sensor IR
/* Requiere: 1 input analogico
 */
#define MINIMO_ESPACIO 100 // minimo valor para determinar si hay algo entre ambos sensores o no 
int entradaIR = A0; // el sensor IR retorna un valor analogico, pero para el proyecto es necesario manejarlo como 1 o 0
bool valIR = false; // TRUE = hay algo; FALSE = no hay nada


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
  Serial.begin(9600);
  servoMotor.attach(servoPin);
  pinMode(entradaIR, INPUT);
  pinMode(entradaBoton, INPUT);
}

void loop()
{
  //
  // INSPECCIONAR TODOS LOS SENSORES ULTRASONICOS
  //
  // Recorrer la lista de multiplexores
  // Por cada elemento del multiplexor, medir la distancia

  // Actualizar la lista del "ESTADO"(Ocupado, Desocupado) de los slots

  // Actualizar estado de los pines
  // Por cada elemento de la lista del "ESTADO" de los slots

  // Actualizar el valor del LED correspondiente

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
      Serial.println("Hola que tal");
    }

    // Verifica que el tiempo que haya pasado con la puerta abierta sea menor al maximo tiempo definido
    if (millis() - capturaTiempo > 10000)
    {
      // Cierra la puerta
      servoMotor.write(0);
      estado = Estado::REPOSO;
    }

    // Si es que se siente algo en el sensor
    if (valIR == true)
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