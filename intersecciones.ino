void intersecciones(){
  while(interseccion<5){
    casos();
  }
  while(interseccion==5){
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
        if(interseccion==5){
          interseccion++;
        }
        SerialBT.print("*** Contador actualizado a: ");
        SerialBT.print(contadorNegro);
        SerialBT.println(" ***");
        
        // Avanzar recto para cruzar la intersección
        motores(60, 60);
        delay(600);
        
        delay(300);
        return;
      } else {
        motores(tp,tp);
      }
  }
  casos();
  
 }

 
