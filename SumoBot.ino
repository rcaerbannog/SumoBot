//Alexander Li
//AL TER4M SumoBot
//Version: 2021-05-13
//Course: TEJ4M1
//Teacher: Mr. Wong
//Function: sumo-wrestling bot that aims to push an opponent bot out of a ring

#include <LiquidCrystal.h>

//Pin variables

//Pin variables
int on_off_pin = 0;
int input1_pin = 1;
int input2_pin = 2;
int input3_pin = 3;
int input4_pin = 4;
int enable12_pin = 5;	//RIGHT motor
int enable34_pin = 6;	//LEFT motor
int uds_left_pin = 7;
int uds_right_pin = A2;
int uds_center_pin = A3;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
int ldrLeft_pin = A4;
int ldrRight_pin = A5;
int dip1_pin = A0;
int dip2_pin = A1;

//Voltage reading for LDRs (drop across second resistor)
int ldrLeft_state = 0, ldrRight_state = 0;

//Distance readings from UDSs
long udsLeft_cm = 0, udsRight_cm = 0, udsCenter_cm = 0;
const int MAX_DIST = 300;

//LCD
long updateLCDTime = 0;

//System mode
const int OFF = 0, INITIAL = 1, SEARCH = 2, TRACK = 3, NEAR_EDGE = 4;
int systemMode = OFF; //0 for idle, 1 for search, 2 for tracking, 3 for facing, 4 for near edge
int previousSystemMode = OFF;

//Motor variables
int defaultSpeed = 0;

//Adjust these for turning
int motorLeftSpeed = 0, motorRightSpeed = 0;	//Speeds from -255 to +255
long turnFinishTime = -1; //-1 if not in use, positive time otherwise. In milliseconds.


void setup() {
  	lcd.begin(16, 2);
	
  	//Pin modes
	pinMode(on_off_pin, INPUT);
	pinMode(input1_pin, OUTPUT);
	pinMode(input2_pin, OUTPUT);
	pinMode(input3_pin, OUTPUT);
	pinMode(input4_pin, OUTPUT);
	pinMode(enable12_pin, OUTPUT);
	pinMode(enable34_pin, OUTPUT);
	pinMode(ldrLeft_pin, INPUT);
	pinMode(ldrRight_pin, INPUT);
	pinMode(dip1_pin, INPUT);
	pinMode(dip2_pin, INPUT);
	
  	//Write enable pins to L293D to high. (Power controlled by duty cycle)
	digitalWrite(enable12_pin, HIGH);
	digitalWrite(enable34_pin, HIGH);
  
  	Serial.end();
}

void loop() {
	//On/Off switch
	if (digitalRead(on_off_pin) == LOW){
		systemMode = OFF;
      	previousSystemMode = systemMode;
		defaultSpeed = 0;
		motorLeftSpeed = 0;
		motorRightSpeed = 0;
		updateMotorStates();
		return;	//skips to next run of loop()
	} 
	if (previousSystemMode == OFF && digitalRead(on_off_pin) == HIGH){	//Robot turned on by switch 4
      	systemMode = INITIAL;
		initialTurning();
	}

	//Update LDRs
	ldrLeft_state = analogRead(ldrLeft_pin);
	ldrRight_state = analogRead(ldrRight_pin);
	//Update LCDs
	udsLeft_cm = updateUDS(uds_left_pin)/58.2377;
	udsRight_cm = updateUDS(uds_right_pin)/58.2377;
	udsCenter_cm = updateUDS(uds_center_pin)/58.2377;

  	//MODE: NEAR EDGE
	//NEAR_EDGE will not be deactivated until escape maneuver has been performed. Revert to search.
	if (ldrLeft_state < 550 || ldrRight_state < 550){
		systemMode = NEAR_EDGE;
		//Hard stop: high impedence by setting enable12_pin and enable34_pin to 0 duty cycle
		motorLeftSpeed = 0;
		motorRightSpeed = 0;
		updateMotorStates();
		delay(200);
      
		//Corrective turn
		if (ldrLeft_state < 550){	//Turn CW (right) 135 degrees
			motorLeftSpeed = 100;
			motorRightSpeed = -100;
			turnFinishTime = millis() + 1500;
		}
		else if (ldrRight_state < 550){	//Turn CCW (left) 135 degrees
			motorLeftSpeed = -100;
			motorRightSpeed = 100;
			turnFinishTime = millis() + 1500;
		}
      
		updateMotorStates();
		delay(turnFinishTime - millis()); //Maintain current motor speeds for duration of turn
		//Travel forwards for 1.5 seconds to get away from edge
		motorLeftSpeed = 200;
		motorRightSpeed = 200;
		turnFinishTime = millis() + 1500;
		updateMotorStates();
		delay(turnFinishTime - millis());
		//Reset system mode
		turnFinishTime = -1;
		systemMode = SEARCH;
	}
  	//MODE: FACING (TRACK)
	//In TinkerCAD, if object is not in range, returned distance is 335, not 0. Normally, timeout on pulseIn would give 0.
	//Parallax Ping))) (#28015) data sheet: https://www.digikey.com/htmldatasheets/production/1253021/0/0/1/28015.html 
	//Min accurate range is 2 cm. By setting UDS back by at least 2cm from edge of car, opposing bot cannot get under sensor range
	//Max accurate range is 3 m (300 cm). All distances beyond will be disregarded.
	else if (udsLeft_cm < 300 || udsCenter_cm < 300 || udsRight_cm < 300){
		systemMode = TRACK;
		defaultSpeed = 255;
		//Opponent sumo bot detected in(F) or (L, F, R) sensors: full forwards
		if (udsCenter_cm < 300 && ((udsLeft_cm < 300 && udsRight_cm < 300) || (udsLeft_cm >= 300 && udsRight_cm >= 300)) ){
			motorLeftSpeed = defaultSpeed;
			motorRightSpeed = defaultSpeed;
		}
      	//Detected in (L, F): turn slight left
		else if (udsLeft_cm < 300 && udsCenter_cm < 300){
			motorLeftSpeed = defaultSpeed - 70;
			motorRightSpeed = defaultSpeed;
		}
      	//Detected in (R, F): turn slight right
		else if (udsRight_cm < 300 && udsCenter_cm < 300){
			motorLeftSpeed = defaultSpeed;
			motorRightSpeed = defaultSpeed - 70;
		}
      	//Detected in (L): turn hard left
		else if (udsLeft_cm < 300){
			motorLeftSpeed = defaultSpeed - 150;
			motorRightSpeed = defaultSpeed;
		}
      	//Detected in (R): turn hard right
		else if (udsRight_cm < 300){
			motorLeftSpeed = defaultSpeed;
			motorRightSpeed = defaultSpeed - 150;
		}
	}
  	//MODE: SEARCH
  	//Continuously rotate CW at slower speed to scan for opponent robot
	else{
		systemMode = SEARCH;
		defaultSpeed = 0;
		motorLeftSpeed = 80;
		motorRightSpeed = -80;
	}

	//Update speeds and direction of each motor
	updateMotorStates();
	previousSystemMode = systemMode;
}

void updateLCD(){
	//Erase display
	lcd.setCursor(0,0);
	lcd.print("                ");
	lcd.setCursor(0,1);
	lcd.print("             ");
  	//Print LDR analog readings in middle
	lcd.setCursor(8, 0);
	lcd.print(ldrLeft_state);
	lcd.setCursor(8, 1);
	lcd.print(ldrRight_state);
  	//Print distance sensor distance readings in left
	lcd.setCursor(0,1);
	lcd.print(udsLeft_cm);
	lcd.setCursor(4,1);
	lcd.print(udsRight_cm);
	lcd.setCursor(2,0);
	lcd.print(udsCenter_cm);
  	lcd.setCursor(13, 0);
  	//Print system mode in top-right
    if (systemMode == OFF){
      	lcd.print("OFF");
    }
    else if (systemMode == INITIAL){
      	lcd.print("INI");
    }
    else if (systemMode == SEARCH){
      	lcd.print("SCH");
    }
    else if (systemMode == TRACK){
      	lcd.print("TRK");
    }
    else if (systemMode == NEAR_EDGE){
      	lcd.print("EDG");
    }
}

//Pulse the ultrasonic distance sensor to get a return time reading to convert to distance
long updateUDS(int pin){
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delayMicroseconds(2);
	digitalWrite(pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(pin, LOW);
	pinMode(pin, INPUT);
	return pulseIn(pin, HIGH, 30000);
}

//Motor methods. 

/* NOT IMPLEMENTED
//Sets the required motor speeds and turn time required to complete a fixed turn
void initiateTurn(int turnDegrees, int turnSpeed){
  //states: turn time has passed, turn time is ongoing, turn time is not in use
  //Add method to quickly flash maximum power: analogWrite(255) to both motors to overcome static friction
}
*/

//Update inputs to the L293D motor control chip to set motor speeds and directions
//Using analogWrite to control the duty cycle of the signal to the Enable pins on the L293D controls effective power to motors
//by quickly enabling/disabling the chip for a certain fraction of each cycle
void updateMotorStates(){
	//Right motor direction and speed
	if (motorRightSpeed < 0){
		digitalWrite(input1_pin, LOW);
		digitalWrite(input2_pin, HIGH);
	}
	else{
		digitalWrite(input1_pin, HIGH);
		digitalWrite(input2_pin, LOW);
	}
	analogWrite(enable12_pin, abs(motorRightSpeed));
	//Left motor direction and speed
	if (motorLeftSpeed < 0){
		digitalWrite(input3_pin, LOW);
		digitalWrite(input4_pin, HIGH);
	}
	else{
		digitalWrite(input3_pin, HIGH);
		digitalWrite(input4_pin, LOW);
	}
	analogWrite(enable34_pin, abs(motorLeftSpeed));
  	//Update the LCD, no more frequently than once every 1/10th of a second to prevent excessie flashing
  	if (millis() >= updateLCDTime){
      	updateLCDTime = millis() + 100;
  		updateLCD();
  	}
}

//Would need mass of car and power curve of motors to make theoretical prediction, or testing
//Note: there is no use in trying to predict required turn times without physical testing
//since I don't know the specs of the wheels or motor, or the weight of the robot
void initialTurning(){
	defaultSpeed = 0;	//If you don't want to turn on the spot for whatever reason, change this
	int dip1_state = analogRead(dip1_pin);
	int dip2_state = analogRead(dip2_pin);
  	lcd.setCursor(13, 1);
  
  	//Conditional statement to specify initial turning angle based on DIP switches 1 and 2
  	//(00, 0), (01, 90), (10, 180), (11, 270). Angles in CW direction.
	if (dip1_state < 500 && dip2_state < 500){
      	lcd.print("000");
		return;	//No turning
	}
	else if (dip1_state < 500 && dip2_state > 500){
		//send commands to motor to turn 90 degrees CW (right)
		motorLeftSpeed = defaultSpeed + 100;
		motorRightSpeed = defaultSpeed - 100;
		turnFinishTime = millis() + 1000;
      	lcd.print("090");
	}
	else if (dip1_state > 500 && dip2_state < 500){
		//send commands to motor to turn 180 degrees CW (right)
		motorLeftSpeed = defaultSpeed + 100;
		motorRightSpeed = defaultSpeed - 100;
		turnFinishTime = millis() + 2000;
      	lcd.print("180");
	}
	else if (dip1_state > 500 && dip2_state > 500){
		//send commands to motor to turn 90 degrees CCW (left)
		motorLeftSpeed = defaultSpeed - 100;
		motorRightSpeed = defaultSpeed + 100;
		turnFinishTime = millis() + 1000;
      	lcd.print("270");
	}
  	updateLCD();	//If this isn't done, LCD won't show correct system mode
	updateMotorStates();
	delay(turnFinishTime-millis());	//Maintain current motor speeds for duration of turn
	turnFinishTime = -1;
}
