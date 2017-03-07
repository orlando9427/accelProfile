#include "AccelProfile.h"

static motor_t motor[MAXMOTOR];
uint8_t motorCount = 0;
volatile uint8_t stepCount = 0;
volatile uint8_t motorTurn = 0;

float dectoRad(long dec) {
	return dec * (PI / 2) / 100;
}

ISR(TIMER1_COMPA_vect) {
	if (stepCount < stepTimer)
		stepCount++;
	else {
		switch (motor[motorTurn].profile.isAccel) {
			case ISSTOP:
				digitalWrite(motor[motorTurn].pinPWM, LOW);
        motor[motorTurn].profile.state = 0;
				break;

			case ISACCEL:
        analogWrite(motor[motorTurn].pinPWM, (int)(motor[motorTurn].profile.vel*sin(dectoRad((100 * motor[motorTurn].profile.state) / motor[motorTurn].profile.accelTime[0]))));
        motor[motorTurn].profile.state += stepTimer;
        if (motor[motorTurn].profile.state >= motor[motorTurn].profile.accelTime[0])
          motor[motorTurn].profile.isAccel = ISSTEADY;
        break;

      case ISSTEADY:
        analogWrite(motor[motorTurn].pinPWM, motor[motorTurn].profile.vel);
        motor[motorTurn].profile.state += stepTimer*2;
        if (motor[motorTurn].profile.state >= motor[motorTurn].profile.accelTime[1])
          motor[motorTurn].profile.isAccel = ISDECCA;
      	break;

      case ISDECCA:
        analogWrite(motor[motorTurn].pinPWM, (int)(motor[motorTurn].profile.vel*sin(dectoRad(100 - (100 * (motor[motorTurn].profile.state - motor[motorTurn].profile.accelTime[1]) / motor[motorTurn].profile.accelTime[0])))));
        motor[motorTurn].profile.state += stepTimer;
        if (motor[motorTurn].profile.state >= motor[motorTurn].profile.accelTime[2])
          motor[motorTurn].profile.isAccel = ISSTOP;
          break;
    }//*/

    motorTurn++;
    if (motorTurn >= MAXMOTOR)
      motorTurn = 0;
    stepCount = 0;
	}
}

AccelProfile::AccelProfile(uint8_t pinForward, uint8_t pinBackward, uint8_t pinPWM, uint8_t part) {
	this->xforward = pinForward;
	this->xbackward = pinBackward;
  this->xPWM = pinPWM;
	this->xpart = part;
	this->motorIndex = motorCount++;

  motor[this->motorIndex].pinM1 = this->xforward;
  motor[this->motorIndex].pinM2 = this->xbackward;
  motor[this->motorIndex].pinPWM = this->xPWM;
  
	pinMode(pinForward, OUTPUT);
	pinMode(pinBackward, OUTPUT);
  pinMode(pinPWM, OUTPUT);
}

void AccelProfile::prepareTimer() {

	//Clear both registers
	TCCR1A = 0;
	TCCR1B = 0;
  TCNT1  = 0;

	//Set CTC mode
	TCCR1B |= (1 << WGM12);

	//Set prescaler to 1024
	//TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);

	//Set Compare Match Register to desired counts
	OCR1A = 3124;	
	//Enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
}

void AccelProfile::moveForward(int vel, int xtime) {
  cli();
  digitalWrite(motor[this->motorIndex].pinM1, HIGH);
  digitalWrite(motor[this->motorIndex].pinM2, LOW);
  Serial.print("M1: ");
  Serial.println(motor[this->motorIndex].pinM1);
  Serial.print("M2: ");
  Serial.println(motor[this->motorIndex].pinM2);
  
	movePlanner(vel, (xtime / 100));
  sei();
}

void AccelProfile::moveBackward(int vel, int xtime) {
  cli();
  digitalWrite(motor[this->motorIndex].pinM2, HIGH);
  digitalWrite(motor[this->motorIndex].pinM1, LOW);
  Serial.print("M1: ");
  Serial.println(motor[this->motorIndex].pinM1);
  Serial.print("M2: ");
  Serial.println(motor[this->motorIndex].pinM2);
  
	movePlanner(vel, (xtime / 100));
  sei();
}

void AccelProfile::movePlanner(int vel, int xtime) {
	//Disable interrupts
	//cli();

	//Stablish data for motor
	//motor[index].pinPWM = xdirection;
	motor[this->motorIndex].profile.vel = vel;
	motor[this->motorIndex].profile.state = 0;
	motor[this->motorIndex].profile.isAccel = ISACCEL;

	motor[this->motorIndex].profile.accelTime[0] = this->accelTimePlanner(xtime);
	motor[this->motorIndex].profile.accelTime[1] = xtime - motor[this->motorIndex].profile.accelTime[0];
	motor[this->motorIndex].profile.accelTime[2] = xtime;
	//sei();

	//Serial.println(motor[index].perfil.accelTime[0]);
}

int AccelProfile::accelTimePlanner(int xtime) {
	return (int)(xtime / this->xpart);
}
