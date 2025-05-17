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
  ESPERA,
  CONSULTA,
  APERTURA,
  CIERRE
} estado = ESPERA;
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
int entradaIR = A0;
int valIR = 0;

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
  valIR = analogRead(entradaIR);
  valBoton = digitalRead(entradaBoton);

  // Guardar el ultimo estado de ser distinto
  bool nuevoEstado = (estado != estadoAnterior);
  estadoAnterior = estado;

  // Comportamiento segun el estado que se encuentre
  switch (estado)
  {
  case Estado::ESPERA: // Estado relacionado al sistema en reposo
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
      estado = Estado::APERTURA;
    }
    else
    {
      estado = Estado::ESPERA;
    }
  }
  break;

  case Estado::APERTURA: // Estado relacionado a abrir la puerta del estacionamiento

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
      unsigned long t = millis() - capturaTiempo;
      Serial.println(t);
      estado = Estado::CIERRE;
    }

    // Verifica que el sensor infrarrojo no haya detectado nada
    // if (valIR)
    // {
    //   servoMotor.write(0);
    // }
    break;

  case Estado::CIERRE: // Estado relacionado a cerrar la puerta del estacionamiento
    // Cierra la puerta con el servomotor
    servoMotor.write(0);
    delay(500);
    estado = Estado::ESPERA;
    break;

  default:
    Serial.println("LOL hubo un error ALGO grave");
    break;
  }

  // Serial.println(valIR);
  // if (valBoton == HIGH)
  // {
  //   Serial.println(valBoton);
  // }
  delay(1000);
}

inline void debug_estados()
{
  switch (estado)
  {
  case Estado::ESPERA:
    Serial.println("Estado::ESPERA");
    break;
  case Estado::CONSULTA:
    Serial.println("Estado::CONSULTA");
    break;
  case Estado::APERTURA:
    Serial.println("Estado::APERTURA");
    break;
  case Estado::CIERRE:
    Serial.println("Estado::CIERRE");
    break;

  default:
    break;
  }

  Serial.print("Anterior > ");
  switch (estadoAnterior)
  {
  case Estado::ESPERA:
    Serial.println("Estado::ESPERA");
    break;
  case Estado::CONSULTA:
    Serial.println("Estado::CONSULTA");
    break;
  case Estado::APERTURA:
    Serial.println("Estado::APERTURA");
    break;
  case Estado::CIERRE:
    Serial.println("Estado::CIERRE");
    break;

  default:
    break;
  }
  Serial.println("");
};
