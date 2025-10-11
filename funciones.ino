/* ---- FUNCIONES DE UTILIDAD ---- */
#include "variable.h"
#include "Adafruit_VL53L0X.h"

extern Adafruit_VL53L0X lox1;
extern MPU6050 mpu;
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

extern BluetoothSerial SerialBT;

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
    
    SerialBT.print("Distancia: ");
    SerialBT.print(distancia);
    SerialBT.println(" mm");
    
    if (distancia < DISTANCIA_MINIMA) {
        SerialBT.println("¡OBSTACULO DETECTADO!");
        return true;
    }
    return false;
}

// Función de maniobra completa de esquive
void maniobrarEsquive() {
    SerialBT.println("\n========================================");
    SerialBT.println("   INICIANDO MANIOBRA DE ESQUIVE");
    SerialBT.println("========================================\n");
    
    // PASO 1: Girar DERECHA 90°
    SerialBT.println("PASO 1: Girando DERECHA 90°...");
    girarDerecha90();
    delay(300);
    
    // PASO 2: Avanzar recto
    SerialBT.println("PASO 2: Avanzando recto...");
    motores(VELOCIDAD_ESQUIVE, VELOCIDAD_ESQUIVE);
    delay(TIEMPO_AVANCE);
    motores(0, 0);
    delay(300);
    
    // PASO 3: Girar IZQUIERDA 90°
    SerialBT.println("PASO 3: Girando IZQUIERDA 90°...");
    girarIzquierda90();
    delay(300);
    
    // PASO 4: Avanzar recto
    SerialBT.println("PASO 4: Avanzando recto...");
    motores(VELOCIDAD_ESQUIVE, VELOCIDAD_ESQUIVE);
    delay(TIEMPO_AVANCE);
    motores(0, 0);
    delay(300);
    
    // PASO 5: Girar IZQUIERDA 90°
    SerialBT.println("PASO 5: Girando IZQUIERDA 90°...");
    girarIzquierda90();
    delay(300);
    
    // PASO 6: Avanzar recto
    SerialBT.println("PASO 6: Avanzando recto...");
    motores(VELOCIDAD_ESQUIVE, VELOCIDAD_ESQUIVE);
    delay(TIEMPO_AVANCE);
    motores(0, 0);
    delay(300);
    
    // PASO 7: DETENER
    SerialBT.println("PASO 7: Deteniendo robot...");
    motores(0, 0);
    delay(500);
    
    // PASO 8: Girar DERECHA 90°
    SerialBT.println("PASO 8: Girando DERECHA 90°...");
    girarDerecha90();
    delay(300);
    
    SerialBT.println("\n========================================");
    SerialBT.println("   MANIOBRA COMPLETADA");
    SerialBT.println("========================================\n");
    
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
    SerialBT.println("Avanzando recto para buscar linea...");
    
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
        SerialBT.println("¡Linea encontrada!");
    } else {
        SerialBT.println("Continuando busqueda...");
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
    izq = constrain(izq, -250, 250);
    der = constrain(der, -250, 250);
    
    motores(izq, der);
}

// Función para girar a la derecha
void girarDerecha90() {
    SerialBT.println("=== PREPARANDO GIRO DERECHA ===");
    
    // Avanzar un poquito antes de girar
    motores(60, 60);
    delay(400);
    motores(0, 0);
    delay(300);
    
    SerialBT.println("Girando...");
    
    // Guardar ángulo inicial
    mpu.update();
    float anguloInicial = mpu.getAngleZ();
    float anguloActual = 0;
    
    motores(-90, 90);
    
    while (anguloActual > -86) {
        mpu.update();
        anguloActual = mpu.getAngleZ() - anguloInicial;
        SerialBT.print("Angulo: ");
        SerialBT.println(anguloActual);
        delay(15);
    }
    
    motores(0, 0);
    delay(100);
    SerialBT.println("Giro completado! Buscando linea...");
    buscarLinea();
}

// Función para girar a la izquierda
void girarIzquierda90() {
    SerialBT.println("=== PREPARANDO GIRO IZQUIERDA ===");
    
    // Avanzar un poquito antes de girar
    motores(60, 60);
    delay(400);
    motores(0, 0);
    delay(300);
    
    SerialBT.println("Girando...");
    
    // Guardar ángulo inicial
    mpu.update();
    float anguloInicial = mpu.getAngleZ();
    float anguloActual = 0;
    
    motores(90, -90);
    
    while (anguloActual < 86) {
        mpu.update();
        anguloActual = mpu.getAngleZ() - anguloInicial;
        SerialBT.print("Angulo: ");
        SerialBT.println(anguloActual);
        delay(15);
    }
    
    motores(0, 0);
    delay(100);
    SerialBT.println("Giro completado! Buscando linea...");
    buscarLinea();
}

void casos() {
    // Leer sensores ANTES de evaluar condiciones
    QTR.read(sensorValues);
    
    // Debug: Imprimir valores cada cierto tiempo
    static unsigned long ultimaPrint = 0;
    if (millis() - ultimaPrint > 500) {
        SerialBT.print("Sensores: ");
        for(int i = 0; i < SensorNum; i++) {
            SerialBT.print(sensorValues[i]);
            SerialBT.print(" ");
        }
        SerialBT.print(" | Contador: ");
        SerialBT.println(contadorNegro);
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
        SerialBT.println(">> INTERSECCION DETECTADA: TODOS EN NEGRO");
        
        contadorNegro++;
        SerialBT.print("*** Contador actualizado a: ");
        SerialBT.print(contadorNegro);
        SerialBT.println(" ***");
        
        // Avanzar recto para cruzar la intersección
        motores(60, 60);
        delay(600);
        
        delay(300);
        
        return;
      }
      // CASO 2: TODOS los sensores detectan BLANCO → AVANZAR RECTO (línea cortada)
      
      // CASO 3: Sensores del 0 al 3 detectan NEGRO (4 sensores) → GIRO IZQUIERDA (ZIG-ZAG)
      if(sensoresNegros < 8 &&
       sensorValues[0] >= blk && 
       sensorValues[1] >= blk && 
       sensorValues[2] >= blk && 
       sensorValues[3] >= blk && sensorValues[4] >= blk) {
        
        SerialBT.println(">> Sensores 0-3 en NEGRO: Confirmando...");
        delay(100);
        
        QTR.read(sensorValues);
        
        sensoresNegros = 0;
        for(int i = 0; i < SensorNum; i++) {
            if(sensorValues[i] >= blk) sensoresNegros++;
        }
        
        if(sensoresNegros < 8 && 
           sensorValues[0] >= blk && sensorValues[1] >= blk && 
           sensorValues[2] >= blk && sensorValues[3] >= blk && sensorValues[4] >= blk) {
            SerialBT.println(">> Confirmado! Girando IZQUIERDA");
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
        
        SerialBT.println(">> Sensores 7-4 en NEGRO: Confirmando...");
        delay(100);
        
        QTR.read(sensorValues);
        
        sensoresNegros = 0;
        for(int i = 0; i < SensorNum; i++) {
            if(sensorValues[i] >= blk) sensoresNegros++;
        }
        
        if(sensoresNegros < 8 && 
           sensorValues[7] >= blk && sensorValues[6] >= blk && 
           sensorValues[5] >= blk && sensorValues[4] >= blk && sensorValues[3] >= blk) {
            SerialBT.println(">> Confirmado! Girando DERECHA");
            girarDerecha90();
            delay(200);
        }
        return;
    } 
      
    
    
    
    
    
    // CASO 5: Seguidor de línea normal con PID
    DynamicPosition();
    PositionFollow();
}
