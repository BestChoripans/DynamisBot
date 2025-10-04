#include "variable.h"

// ------- MOTORES ------- //

void MotorSetup(){
    pinMode(BIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    ledcSetup(1, freq, resolution);
    ledcAttachPin(PWMB, 1);
    
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    ledcSetup(0, freq, resolution);
    ledcAttachPin(PWMA, 0);
}

// accionamiento motor izquierdo
void Motoriz(int value) {
    if ( value >= 0 ) {
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
    } else {
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        value *= -1;
    }
    ledcWrite(0, value);
}

// Función accionamiento motor derecho
void Motorde(int value) {
    if ( value >= 0 ) {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
    } else {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        value *= -1;
    }
    ledcWrite(1, value);
}

// accionamiento motor entero
void motores(int velizq, int velder){
    Motoriz(velizq);
    Motorde(velder);
}
