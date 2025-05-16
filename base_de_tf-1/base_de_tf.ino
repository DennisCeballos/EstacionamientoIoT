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

// Variables del sistema
/* 
*/

// Setup de variables
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servoMotor.attach(servoPin);
  pinMode(entradaIR, INPUT);
  pinMode(entradaBoton, INPUT);
}


void loop() {
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
  //
  // IMPLEMENTACION DE AUTOMATA DE ESTADOS FINITOS
  valIR = analogRead(entradaIR);
  valBoton = digitalRead(entradaBoton);

  if ( valIR<100 ){
    servoMotor.write(0);
    delay(1000);
    servoMotor.write(90);
    delay(1000);
    servoMotor.write(180);
    delay(1000);
  }

  Serial.println(valIR);
  if (valBoton == HIGH){
    Serial.println(valBoton);
  }
  delay(500);

}
