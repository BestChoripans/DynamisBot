#include <QTRSensors.h>
#include <MPU6050_light.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>  // ← NUEVA LIBRERÍA
#include "variable.h"

MPU6050 mpu(Wire);
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();  // ← NUEVO SENSOR

QTRSensors QTR;
int posicion;
int blk = 3350;
const uint8_t SensorNum = 8;
uint16_t sensorValues[SensorNum];
const int freq = 5000;
const int resolution = 8;
const float kp = 0.153;
const int tp = 25;
int error;
int izq;
int der;
int giro;
int contadorNegro = 0;
int interseccion = 0;
// 5 intersecciones antes de la decision

void setup() {
    // Configurar motores
    pinMode(BIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    ledcSetup(1, freq, resolution);
    ledcAttachPin(PWMB, 1);
    
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    ledcSetup(0, freq, resolution);
    ledcAttachPin(PWMA, 0);
    
    Serial.begin(115200);
    Wire.begin();
    
    // INICIALIZAR MPU6050
    Serial.println("Calibrando MPU6050...");
    mpu.begin();
    mpu.calcGyroOffsets();
    Serial.println("MPU6050 calibrado!");
    
    // INICIALIZAR VL53L0X ← NUEVO
    Serial.println("Inicializando VL53L0X...");
    if (!lox1.begin()) {
        Serial.println(F("Failed to boot VL53L0X"));
        while(1);
    }
    Serial.println("VL53L0X iniciado!");
    
    // CONFIGURAR QTR
    QTR.setTypeAnalog();
    QTR.setSensorPins((const uint8_t[]){36, 39, 34, 35, 32, 33, 25, 26}, SensorNum);
    QTR.setEmitterPin(27);
    
    pinMode(LED, OUTPUT);
    pinMode(BOTON, INPUT);
    
    digitalWrite(LED, HIGH);
    
    Serial.println("Calibrando QTR (mueve el robot sobre la linea)...");
    for(int i = 0; i < 200; i++) {
        QTR.calibrate();
        delay(10);
    }
    Serial.println("QTR calibrado!");
    
    digitalWrite(LED, LOW);
    
    Serial.println("Presiona el boton para iniciar...");
    while(digitalRead(BOTON) == 0) {
        delay(10);
    }
    Serial.println("INICIANDO!");
    delay(1000);
}

void loop() {
    checkObstaculo();  // ← PRIMERO verifica obstáculos
    intersecciones();           // ← LUEGO sigue la lín  ea
}