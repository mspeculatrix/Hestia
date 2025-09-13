#include <Servo.h>

#define HEAD_SERVO_PAN_PIN 9
#define HEAD_SERVO_TILT_PIN 10
#define SPARE_SERVO_PIN 5

#define HEAD_PAN_LEFT_MAX 178
#define HEAD_PAN_RIGHT_MAX 7
#define HEAD_PAN_CENTRE 92

#define TILT_BACK_MAX 120
#define TILT_FORWARD_MAX 50
#define TILT_NEUTRAL 81

Servo headPanServo;
Servo headTiltServo;
Servo spareServo;

void setup() {
	Serial.begin(19200);
	headPanServo.attach(HEAD_SERVO_PAN_PIN);
	headTiltServo.attach(HEAD_SERVO_TILT_PIN);
	spareServo.attach(SPARE_SERVO_PIN);

	headPanServo.write(90);

	/*for(byte i=TILT_FORWARD_MAX; i <= TILT_BACK_MAX; i++) {
		headTiltServo.write(i);
   //     spareServo.write(i);
   //     headPanServo.write(i);
		Serial.println(i);
		delay(30);
	}*/


	//spareServo.write(90);
   // headPanServo.write(PAN_CENTRE);


/*
  headPanServo.write(180);
  headTiltServo.write(180);
  */
}


void loop() {
	headTiltServo.write(TILT_NEUTRAL);
	/*
	headTiltServo.write(TILT_FORWARD_MAX);
	delay(5000);
	headTiltServo.write(TILT_BACK_MAX);
	delay(5000);
	headTiltServo.write(TILT_NEUTRAL);
	delay(5000);
  */

	headPanServo.write(HEAD_PAN_LEFT_MAX);
	delay(5000);
	headPanServo.write(HEAD_PAN_RIGHT_MAX);
	delay(5000);
	headPanServo.write(HEAD_PAN_CENTRE);
	delay(5000);
}
