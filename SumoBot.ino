#include <LiquidCrystal.h>

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
	if (previousSystemMode == OFF && digitalRead(on_off_pin) == HIGH){
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

	//NEAR_EDGE mode will not be deactivated until escape maneuver has been performed. Revert to search.
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
		delay(turnFinishTime - millis());
		//Travel forwards for 1 second to get away from edge
		motorLeftSpeed = 200;
		motorRightSpeed = 200;
		turnFinishTime = millis() + 1000;
		updateMotorStates();
		delay(turnFinishTime - millis());
		//Reset
		turnFinishTime = -1;
		systemMode = SEARCH;
	}
	//In TinkerCAD, if object is not in range, returned distance is 335, not 0. Normally, timeout on pulseIn would give 0.
	//Parallax Ping))) (#28015) data sheet: https://www.digikey.com/htmldatasheets/production/1253021/0/0/1/28015.html 
	//Min accurate range is 2 cm. By setting UDS back by at least 2cm from edge of car, opposing bot cannot get under sensor range
	//Max accurate range is 3 m (300 cm). All distances beyond will be disregarded.
	else if (udsLeft_cm < 300 || udsCenter_cm < 300 || udsRight_cm < 300){
		systemMode = TRACK;
		defaultSpeed = 255;
		//only in center or detected by all 3
		if (udsCenter_cm < 300 && ((udsLeft_cm < 300 && udsRight_cm < 300) || (udsLeft_cm >= 300 && udsRight_cm >= 300)) ){
			motorLeftSpeed = defaultSpeed;
			motorRightSpeed = defaultSpeed;
		}
		else if (udsLeft_cm < 300 && udsCenter_cm < 300){	//Turn soft left
			motorLeftSpeed = defaultSpeed - 70;
			motorRightSpeed = defaultSpeed;
		}
		else if (udsRight_cm < 300 && udsCenter_cm < 300){	//Turn soft right
			motorLeftSpeed = defaultSpeed;
			motorRightSpeed = defaultSpeed - 70;
		}
		else if (udsLeft_cm < 300){	//Turn hard left
			motorLeftSpeed = defaultSpeed - 150;
			motorRightSpeed = defaultSpeed;
		}
		else if (udsRight_cm < 300){	//Turn hard right
			motorLeftSpeed = defaultSpeed;
			motorRightSpeed = defaultSpeed - 150;
		}
	}
	else{
		systemMode = SEARCH;	//Rotate continuously CW
		defaultSpeed = 0;
		motorLeftSpeed = 100;
		motorRightSpeed = -100;
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
	lcd.setCursor(8, 0);
	lcd.print(ldrLeft_state);
	lcd.setCursor(8, 1);
	lcd.print(ldrRight_state);
	lcd.setCursor(0,1);
	lcd.print(udsLeft_cm);
	lcd.setCursor(4,1);
	lcd.print(udsRight_cm);
	lcd.setCursor(2,0);
	lcd.print(udsCenter_cm);
	//Columns 13,14,15 are usable. OFF, SCH, TRK, ATK, EDG
  	lcd.setCursor(13, 0);
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

//Update UDSs
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
}
*/

//positive rpm on motors indicates wheel spinning forwards
void updateMotorStates(){
	//Right motor
	if (motorRightSpeed < 0){
		digitalWrite(input1_pin, LOW);
		digitalWrite(input2_pin, HIGH);
	}
	else{
		digitalWrite(input1_pin, HIGH);
		digitalWrite(input2_pin, LOW);
	}
	analogWrite(enable12_pin, motorRightSpeed);
	//Left motor
	if (motorLeftSpeed < 0){
		digitalWrite(input3_pin, LOW);
		digitalWrite(input4_pin, HIGH);
	}
	else{
		digitalWrite(input3_pin, HIGH);
		digitalWrite(input4_pin, LOW);
	}
	analogWrite(enable34_pin, motorLeftSpeed);
  	if (millis() >= updateLCDTime){
  		updateLCD();
    	updateLCDTime = millis() + 100;
  	}
}

//Note: there is no use in trying to predict required turn times without testing
//Would need mass of car and power curve of motors to make theoretical prediction, or testing
void initialTurning(){
	defaultSpeed = 0;	//If you don't want to turn on the spot for whatever reason, change this
	int dip1_state = analogRead(dip1_pin);
	int dip2_state = analogRead(dip2_pin);
  	lcd.setCursor(13, 1);
	if (dip1_state < 500 && dip2_state < 500){
      	lcd.print("000");
		return;	//No turning
	}
	else if (dip1_state < 500 && dip2_state > 500){
		//send commands to motor to turn 90 degrees CW (right)
		motorLeftSpeed = defaultSpeed - 100;
		motorRightSpeed = defaultSpeed + 100;
		turnFinishTime = millis() + 1000;
      	lcd.print("090");
	}
	else if (dip1_state > 500 && dip2_state < 500){
		//send commands to motor to turn 180 degrees CW (right)
		motorLeftSpeed = defaultSpeed - 100;
		motorRightSpeed = defaultSpeed + 100;
		turnFinishTime = millis() + 2000;
      	lcd.print("180");
	}
	else if (dip1_state > 500 && dip2_state > 500){
		//send commands to motor to turn 90 degrees CCW (left)
		motorLeftSpeed = defaultSpeed + 100;
		motorRightSpeed = defaultSpeed - 100;
		turnFinishTime = millis() + 1000;
      	lcd.print("270");
	}
	updateMotorStates();
	delay(turnFinishTime-millis());
	turnFinishTime = -1;
}
