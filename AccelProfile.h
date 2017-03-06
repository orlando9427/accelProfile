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
#define stepTimer steps / 2


typedef struct {
	long accelTime[3];
	int vel;
	long state;
	int isAccel;
} profile_t;

typedef struct {
	profile_t profile;
	uint8_t pin;
} motor_t;

class AccelProfile {
	public:
		AccelProfile(uint8_t pinForward, uint8_t pinBackward, uint8_t part);

		void prepareTimer();
		void moveForward(int vel, int xtime);
		void moveBackward(int vel, int xTime);
	private:
		int xpart, xforward, xbackward, motorIndex;

		int accelTimePlanner(int xtime);
		void movePlanner(uint8_t index, int xdirection, int vel, int xtime);
};
