#include "AccelProfile.h"

volatile motor_t motor[2];
uint8_t motorCount = 0;
volatile int stepCount = 0;
volatile uint8_t motorTurn = 0;

float dectoRad(long dec) {
	return dec * (M_PI / 2) / 100;
}

ISR(TIMER1_COMPA_vect) {
	if (stepCount < stepTimer)
		stepCount++;
	else {
		switch (motor[motorTurn].profile.isAccel) {
			case ISSTOP:
				digitalWrite(motor[motorTurn].pin, LOW);
				break;

			case ISACCEL:
        analogWrite(motor[motorCount].pin, (int)motor[motorCount].profile.vel*sin(dectoRad((100 * motor[motorCount].profile.state) / motor[motorCount].profile.accelTime[0])));
        motor[motorCount].profile.state += stepTimer;
        if (motor[motorCount].profile.state >= motor[motorCount].profile.accelTime[0])
          motor[motorCount].profile.isAccel = ISSTEADY;
        break;

      case ISSTEADY:
        analogWrite(motor[motorCount].pin, motor[motorCount].profile.vel);
        motor[motorCount].profile.state += stepTimer;
        if (motor[motorCount].profile.state >= motor[motorCount].profile.accelTime[1])
          motor[motorCount].profile.isAccel = ISDECCA;
      	break;

      case ISDECCA:
        analogWrite(motor[motorCount].pin, (int)motor[motorCount].profile.vel*sin(dectoRad(100 - (100 * (motor[motorCount].profile.state - motor[motorCount].profile.accelTime[1]) / motor[motorCount].profile.accelTime[0]))));
        motor[motorCount].profile.state += stepTimer;
        if (motor[motorCount].profile.state >= motor[motorCount].profile.accelTime[2])
          motor[motorCount].profile.isAccel = ISSTOP;
          break;
    }

    motorTurn++;
    if (motorTurn >= MAXMOTOR)
      motorTurn = 0;
    stepCount = 0;
	}
}

AccelProfile::AccelProfile(byte pinForward, byte pinBackward, byte part) {
	this->xforward = pinForward;
	this->xbackward = pinBackward;
	this->xpart = part;
	this->motorIndex = motorCount++;

	pinMode(this->xforward, OUTPUT);
	pinMode(this->xbackward, OUTPUT);
}

void AccelProfile::prepareTimer() {

	//Clear both registers
	TCCR1A = 0;
	TCCR1B = 0;

	//Set CTC mode
	TCCR1B |= (1 << WGM12);

	//Set prescaler to 1024
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);

	//Set Compare Match Register to desired counts
	OCR1A = 31249;	
	//Enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
}

void AccelProfile::moveForward(int vel, int xtime) {
	movePlanner(this->motorIndex, this->xforward, vel, xtime / 100);
}

void AccelProfile::moveBackward(int vel, int xtime) {
	movePlanner(this->motorIndex, this->xbackward, vel, xtime / 100);
}

void AccelProfile::movePlanner(byte index, int xdirection, int vel, int xtime) {
	//Disable interrupts
	cli();

	//Stablish data for motor
	motor[index].pin = xdirection;
	motor[index].profile.vel = vel;
	motor[index].profile.state = 0;
	motor[index].profile.isAccel = ISACCEL;

	motor[index].profile.accelTime[0] = this->accelTimePlanner(xtime);
	motor[index].profile.accelTime[1] = xtime - motor[index].profile.accelTime[0];
	motor[index].profile.accelTime[2] = xtime;
	sei();

	//Serial.println(motor[index].perfil.accelTime[0]);
}

int AccelProfile::accelTimePlanner(int xtime) {
	return (int)(xtime / this->xpart);
}
