void ReadQTR() {
  QTR.read(sensorValues);
  for(int i=0;i<8;i++){
    if(sensorValues[i]>blk){
      SerialBT.print("□ (");
      SerialBT.print(sensorValues[i]);
      SerialBT.print(")");
    } else {
      SerialBT.print("■ (");
      SerialBT.print(sensorValues[i]);
      SerialBT.print(")");
    } 
    SerialBT.print("\t");
  }
  SerialBT.println();
  delay(100);
}


void testmotor(){
  motores(100, 100); // codigo para el movimiento de los motores
  delay(500);
  motores(0,50);
  delay(500);
  motores(50,0);
}

void testgiro(){
  mpu.update();  // Actualizar los datos del sensor
  int angulo = mpu.getAngleZ(); //Obtiene el ángulo actual del eje solicitado
  SerialBT.println(angulo);
}
