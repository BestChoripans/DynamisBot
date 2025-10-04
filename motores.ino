#include "variable.h"

// ------- MOTORES ------- //

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
