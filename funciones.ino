/* ---- FUNCIONES DE UTILIDAD ---- */
#include "variable.h"
#include "Adafruit_VL53L0X.h"

extern Adafruit_VL53L0X lox1;
extern MPU6050 mpu;
extern BluetoothSerial SerialBT;
extern QTRSensors QTR;
extern int posicion;
extern int blk;
extern const uint8_t SensorNum;
extern uint16_t sensorValues[];
extern const float kp;
extern const int tp;
extern int error;
extern int izq;
extern int der;
extern int giro;
extern int contadorNegro;

// ===== CONFIGURACIÓN DEL SENSOR VL53L0X =====
#define DISTANCIA_MINIMA 105  // Distancia en mm para detectar obstáculo (ajusta según necesites)
#define TIEMPO_AVANCE 800     // Tiempo que avanza recto en cada tramo (ms)
#define VELOCIDAD_ESQUIVE 70  // Velocidad durante la maniobra

// ===== FUNCIONES VL53L0X =====

// Función para leer distancia del láser
int leerDistancia() {
    VL53L0X_RangingMeasurementData_t measure;
    lox1.rangingTest(&measure, false);
    
    if (measure.RangeStatus != 4) {  // Si la medición es válida
        return measure.RangeMilliMeter;
    } else {
        return 8000;  // Retorna un valor alto si está fuera de rango
    }
}

// Función para detectar obstáculo
bool detectarObstaculo() {
    int distancia = leerDistancia();
    
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" mm");
    
    if (distancia < DISTANCIA_MINIMA) {
        Serial.println("¡OBSTACULO DETECTADO!");
        return true;
    }
    return false;
}

// Función de maniobra completa de esquive
void maniobrarEsquive() {
    Serial.println("\n========================================");
    Serial.println("   INICIANDO MANIOBRA DE ESQUIVE");
    Serial.println("========================================\n");
    
    // PASO 1: Girar DERECHA 90°
    Serial.println("PASO 1: Girando DERECHA 90°...");
    girarDerecha90();
    delay(300);
    
    // PASO 2: Avanzar recto
    Serial.println("PASO 2: Avanzando recto...");
    motores(VELOCIDAD_ESQUIVE, VELOCIDAD_ESQUIVE);
    delay(TIEMPO_AVANCE);
    motores(0, 0);
    delay(300);
    
    // PASO 3: Girar IZQUIERDA 90°
    Serial.println("PASO 3: Girando IZQUIERDA 90°...");
    girarIzquierda90();
    delay(300);
    
    // PASO 4: Avanzar recto
    Serial.println("PASO 4: Avanzando recto...");
    motores(VELOCIDAD_ESQUIVE, VELOCIDAD_ESQUIVE);
    delay(TIEMPO_AVANCE);
    motores(0, 0);
    delay(300);
    
    // PASO 5: Girar IZQUIERDA 90°
    Serial.println("PASO 5: Girando IZQUIERDA 90°...");
    girarIzquierda90();
    delay(300);
    
    // PASO 6: Avanzar recto
    Serial.println("PASO 6: Avanzando recto...");
    motores(VELOCIDAD_ESQUIVE, VELOCIDAD_ESQUIVE);
    delay(TIEMPO_AVANCE);
    motores(0, 0);
    delay(300);
    
    // PASO 7: DETENER
    Serial.println("PASO 7: Deteniendo robot...");
    motores(0, 0);
    delay(500);
    
    // PASO 8: Girar DERECHA 90°
    Serial.println("PASO 8: Girando DERECHA 90°...");
    girarDerecha90();
    delay(300);
    
    Serial.println("\n========================================");
    Serial.println("   MANIOBRA COMPLETADA");
    Serial.println("========================================\n");
    
    // Pausa final antes de retomar seguimiento de línea
    delay(500);
}

// Función para verificar y ejecutar esquive (llamar en loop)
void checkObstaculo() {
    if (detectarObstaculo()) {
        maniobrarEsquive();
    }
}

void buscarLinea() {
    Serial.println("Avanzando recto para buscar linea...");
    
    // Avanzar recto por 1 segundo
    motores(80, 80);
    delay(300);
    motores(0, 0);
    delay(100);
    
    // Verificar si encontró la línea
    QTR.read(sensorValues);
    bool lineaEncontrada = false;
    
    for (int i = 0; i < SensorNum; i++) {
        if (sensorValues[i] >= blk) {
            lineaEncontrada = true;
            break;
        }
    }
    
    if (lineaEncontrada) {
        Serial.println("¡Linea encontrada!");
    } else {
        Serial.println("Continuando busqueda...");
    }
}


// ===== FUNCIONES ORIGINALES DEL ROBOT =====

void DynamicPosition(){
    QTR.read(sensorValues);
    posicion = QTR.readLineBlack(sensorValues);
    posicion = map(posicion, 0, 7000, -255, 255);
    
    SerialBT.print("Posicion: ");
    SerialBT.println(posicion);
    error = posicion;
}

void PositionFollow(){
    giro = kp * error;
    izq = tp + giro;
    der = tp - giro;
    
    // Limitar velocidades para evitar valores fuera de rango
    izq = constrain(izq, -255, 255);
    der = constrain(der, -255, 255);
    
    motores(izq, der);
}

// Función para girar a la derecha
void girarDerecha90() {
    Serial.println("=== PREPARANDO GIRO DERECHA ===");
    
    // Avanzar un poquito antes de girar
    motores(60, 60);
    delay(400);
    motores(0, 0);
    delay(300);
    
    Serial.println("Girando...");
    
    // Guardar ángulo inicial
    mpu.update();
    float anguloInicial = mpu.getAngleZ();
    float anguloActual = 0;
    
    motores(-90, 90);
    
    while (anguloActual > -86) {
        mpu.update();
        anguloActual = mpu.getAngleZ() - anguloInicial;
        Serial.print("Angulo: ");
        Serial.println(anguloActual);
        delay(15);
    }
    
    motores(0, 0);
    delay(100);
    Serial.println("Giro completado! Buscando linea...");
    buscarLinea();
}

// Función para girar a la izquierda
void girarIzquierda90() {
    Serial.println("=== PREPARANDO GIRO IZQUIERDA ===");
    
    // Avanzar un poquito antes de girar
    motores(60, 60);
    delay(400);
    motores(0, 0);
    delay(300);
    
    Serial.println("Girando...");
    
    // Guardar ángulo inicial
    mpu.update();
    float anguloInicial = mpu.getAngleZ();
    float anguloActual = 0;
    
    motores(90, -90);
    
    while (anguloActual < 86) {
        mpu.update();
        anguloActual = mpu.getAngleZ() - anguloInicial;
        Serial.print("Angulo: ");
        Serial.println(anguloActual);
        delay(15);
    }
    
    motores(0, 0);
    delay(100);
    Serial.println("Giro completado! Buscando linea...");
    buscarLinea();
}

void casos() {
    // Leer sensores ANTES de evaluar condiciones
    QTR.read(sensorValues);
    
    // Debug: Imprimir valores cada cierto tiempo
    static unsigned long ultimaPrint = 0;
    if (millis() - ultimaPrint > 500) {
        Serial.print("Sensores: ");
        for(int i = 0; i < SensorNum; i++) {
            Serial.print(sensorValues[i]);
            Serial.print(" ");
        }
        Serial.print(" | Contador: ");
        Serial.println(contadorNegro);
        ultimaPrint = millis();
    }
    
    // Contar cuántos sensores detectan negro y blanco
    int sensoresNegros = 0;
    int sensoresBlancos = 0;
    
    for(int i = 0; i < SensorNum; i++) {
        if(sensorValues[i] >= blk) {
            sensoresNegros++;
        } else {
            sensoresBlancos++;
        }
    }
    
    // CASO 1 (MÁXIMA PRIORIDAD): TODOS los sensores detectan NEGRO → AVANZAR RECTO y CONTAR
    if(sensoresNegros == 8) {
        Serial.println(">> INTERSECCION DETECTADA: TODOS EN NEGRO");
        
        contadorNegro++;
        Serial.print("*** Contador actualizado a: ");
        Serial.print(contadorNegro);
        Serial.println(" ***");
        
        // Avanzar recto para cruzar la intersección
        motores(60, 60);
        delay(600);
        
        delay(300);
        
        return;
      }
      // CASO 2: TODOS los sensores detectan BLANCO → AVANZAR RECTO (línea cortada)
    if(sensoresBlancos == 8) {
        Serial.println(">> TODOS EN BLANCO: Avanzando recto (linea cortada)");
        motores(tp, tp);
        delay(50);
        return;
    }

      
      // CASO 3: Sensores del 0 al 3 detectan NEGRO (4 sensores) → GIRO IZQUIERDA (ZIG-ZAG)
      if(sensoresNegros < 8 &&
       sensorValues[0] >= blk && 
       sensorValues[1] >= blk && 
       sensorValues[2] >= blk && 
       sensorValues[3] >= blk && sensorValues[4] >= blk) {
        
        Serial.println(">> Sensores 0-3 en NEGRO: Confirmando...");
        delay(100);
        
        QTR.read(sensorValues);
        
        sensoresNegros = 0;
        for(int i = 0; i < SensorNum; i++) {
            if(sensorValues[i] >= blk) sensoresNegros++;
        }
        
        if(sensoresNegros < 8 && 
           sensorValues[0] >= blk && sensorValues[1] >= blk && 
           sensorValues[2] >= blk && sensorValues[3] >= blk && sensorValues[4] >= blk) {
            Serial.println(">> Confirmado! Girando IZQUIERDA");
            girarIzquierda90();
            delay(200);
        }
        return;
    } 
    
    // CASO 4: Sensores del 7 al 4 detectan NEGRO (4 sensores) → GIRO DERECHA (ZIG-ZAG)
    if(sensoresNegros < 8 &&
       sensorValues[7] >= blk && 
       sensorValues[6] >= blk && 
       sensorValues[5] >= blk && 
       sensorValues[4] >= blk && sensorValues[3] >= blk) {
        
        Serial.println(">> Sensores 7-4 en NEGRO: Confirmando...");
        delay(100);
        
        QTR.read(sensorValues);
        
        sensoresNegros = 0;
        for(int i = 0; i < SensorNum; i++) {
            if(sensorValues[i] >= blk) sensoresNegros++;
        }
        
        if(sensoresNegros < 8 && 
           sensorValues[7] >= blk && sensorValues[6] >= blk && 
           sensorValues[5] >= blk && sensorValues[4] >= blk && sensorValues[3] >= blk) {
            Serial.println(">> Confirmado! Girando DERECHA");
            girarDerecha90();
            delay(200);
        }
        return;
    } 
    
    
    
    
    
    
    // CASO 5: Seguidor de línea normal con PID
    DynamicPosition();
    PositionFollow();
}
