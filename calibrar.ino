void QTR_Setup(){
    digitalWrite(LED, HIGH);
    
    SerialBT.println("Calibrando QTR (mueve el robot sobre la linea)...");
    for(int i = 0; i < 200; i++) {
        QTR.calibrate();
        delay(10);
    }
    SerialBT.println("QTR calibrado!");
    
    digitalWrite(LED, LOW);
    
    SerialBT.println("Presiona el boton para iniciar...");
    while(digitalRead(BOTON) == 0) {
        delay(10);
    }
    SerialBT.println("INICIANDO!");
    delay(1000);
}

void MPU6050_Setup(){
    Wire.begin();
    
    // INICIALIZAR MPU6050
    SerialBT.println("Calibrando MPU6050...");
    mpu.begin();
    mpu.calcGyroOffsets();
    SerialBT.println("MPU6050 calibrado!");
}

void VL5_Setup(){
    // INICIALIZAR VL53L0X ← NUEVO
    SerialBT.println("Inicializando VL53L0X...");
    if (!lox1.begin()) {
        SerialBT.println(F("Failed to boot VL53L0X"));
        while(1);
    }
    SerialBT.println("VL53L0X iniciado!");
}
