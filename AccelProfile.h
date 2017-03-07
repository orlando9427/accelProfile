#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include <inttypes.h>
#include <math.h>


// Motor status
#define ISSTOP 0
#define ISACCEL 1
#define ISSTEADY 2
#define ISDECCA 3

//Steps timer1
#define MAXMOTOR 2
#define steps 2
#define stepTimer (steps / 2)


typedef volatile struct {
	long accelTime[3];
	int vel;
	long state;
	int isAccel;
} profile_t;

typedef volatile struct {
	profile_t profile;
	uint8_t pinPWM;
  uint8_t pinM1;
  uint8_t pinM2;
} motor_t;

class AccelProfile {
	public:
		AccelProfile(uint8_t pinForward, uint8_t pinBackward, uint8_t pinPWM,  uint8_t part);

		void prepareTimer();
		void moveForward(int vel, int xtime);
		void moveBackward(int vel, int xTime);
	private:
		int xpart, xforward, xbackward, xPWM, motorIndex;

		int accelTimePlanner(int xtime);
		void movePlanner(int vel, int xtime);
};
