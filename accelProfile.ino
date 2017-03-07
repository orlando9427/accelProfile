#include "AccelProfile.h"

#define MAX_US 256
#define MIN_US 0
#define PWMA 3
#define PWMB 11
#define AIN1 5
#define AIN2 4
#define BIN1 7
#define BIN2 8

struct structOrder {
  char idOrder;
  String dataOrder;
};

int iPos;
AccelProfile motorLeft(AIN1, AIN2, PWMA, 4);
AccelProfile motorRight(BIN1, BIN2, PWMB, 4);


void setup() {
  Serial.begin(9600);
  Serial.println("Hi!");
  motorLeft.prepareTimer();
}

void loop() {
  while (!Serial) {}
  
  //Serial.print("Hola");
  structOrder Order;
  int uS;
  Order.dataOrder = "";

  if (Serial.available() > 0) {
    Order.idOrder = Serial.read();
    do {
      char inChar = Serial.read();
      Order.dataOrder += inChar;
    } while (Serial.available() > 0);
  }
  
  switch (Order.idOrder) {
    case '1':
      uS = Order.dataOrder.toInt();
      if ((uS > MIN_US) and (uS < MAX_US)) {
        Serial.print("1-Izq. ");
        Serial.println(uS);
        motorLeft.moveForward(uS, 1200);
        //startMotor();
      }
      break;
      
     case '2':
      uS = Order.dataOrder.toInt();
      if ((uS > MIN_US) and (uS < MAX_US)) {
        Serial.print("2-Der. ");
        Serial.println(uS);
        motorRight.moveForward(uS, 1200);
      }
      break;
 
    case '3':
      uS = Order.dataOrder.toInt();
      if ((uS > MIN_US) and (uS < MAX_US)) {
        Serial.print("3-Der. ");
        Serial.println(uS);
        motorLeft.moveBackward(uS, 1200);
      }
      break;
      
    case '4':
      uS = Order.dataOrder.toInt();
      if ((uS > MIN_US) and (uS < MAX_US)) {
        Serial.print("4-Der. ");
        Serial.println(uS);
        motorRight.moveBackward(uS, 1200);
      }
      break;

    case '5':
      uS = Order.dataOrder.toInt();
      if ((uS > MIN_US) and (uS < MAX_US)) {
        Serial.print("5-Atr. ");
        Serial.println(uS);
        motorRight.moveForward(uS, 6000);
        motorLeft.moveForward(uS, 6000);
      }
      break;
      
    case '6':
      uS = Order.dataOrder.toInt();
      if ((uS > MIN_US) and (uS < MAX_US)) {
        Serial.print("6-Ade. ");
        Serial.println(uS);
        motorRight.moveBackward(uS, 6000);
        motorLeft.moveBackward(uS, 6000);
        //startMotor();
      }
      break;

    case '7':
      Serial.println("Prueba");
      pinMode(PWMB, OUTPUT);
      analogWrite(PWMB, (int)random()*255);
      pinMode(PWMA, OUTPUT);
      analogWrite(PWMA, (int)random()*255);
  }
  //Serial.flush();
  delay(1000);
}

/*void probeMotor(){
  analogWrite(AIN1, 0);
  analogWrite(AIN2, 0);
  for (iPos = MIN_US; iPos <= MAX_US; iPos += 16) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    analogWrite(M1R, iPos);
    analogWrite(M2R, iPos);
    delay(500);                       
  }

  analogWrite(M1R, 0);
  analogWrite(M2R, 0);
  for (iPos = MAX_US; iPos >= MIN_US; iPos -= 16) { // goes from 180 degrees to 0 degrees
    analogWrite(M1R, iPos);
    analogWrite(M2R, iPos);
    delay(500);                       // waits 15ms for the servo to reach the position
  }
}*/

