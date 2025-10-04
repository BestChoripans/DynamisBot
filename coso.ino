#include <QTRSensors.h>
#include <MPU6050_light.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "variable.h"

/* inicializar variables de librerias */
MPU6050 mpu(Wire);
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
QTRSensors QTR;

/* Variables de Seguidor de linea y demas*/
int posicion;
int blk = 3350;

/* Arreglos */
const uint8_t SensorNum = 8;
uint16_t sensorValues[SensorNum];

/* Config ESP32 */
const int freq = 5000;
const int resolution = 8;

/* PID */
const float kp = 0.153;
const int tp = 25;
int error;
int izq;
int der;
int giro;
int contadorNegro = 0;
int interseccion = 0;

void setup() {
    // Configurar motores
    MotorSetup();
    
    Serial.begin(115200);

    /* Giroscopio */
    MPU6050_Setup();

    /* Laser */
    VL5_Setup();
    
    // CONFIGURAR QTR
    QTR.setTypeAnalog();
    QTR.setSensorPins((const uint8_t[]){36, 39, 34, 35, 32, 33, 25, 26}, SensorNum);
    QTR.setEmitterPin(27);
    
    /* LED SETUP */
    pinMode(LED, OUTPUT);
    pinMode(BOTON, INPUT);
    
    /* Calibracion QTR inicial */
    calibrarqtr();
}

void loop() {
    checkObstaculo();  // ← PRIMERO verifica obstáculos
    intersecciones(); // ← LUEGO sigue la línea
}