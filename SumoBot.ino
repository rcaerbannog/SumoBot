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
int ldr1_pin = A4;
int ldr2_pin = A5;
int dip1_pin = A0;
int dip2_pin = A1;

//Voltage reading for LDRs (drop across second resistor)
int ldr1_state = 0, ldr2_state = 0;
//Distance readings from UDSs
long udsLeft_cm = 0, udsRight_cm = 0, udsCenter_cm = 0;
const int MAX_DIST = 300;

//System mode
const int OFF = 0, SEARCH = 1, TRACK = 2, ATTACK = 3, NEAR_EDGE = 4;
int systemMode = 0; //0 for idle, 1 for search, 2 for tracking, 3 for facing, 4 for near edge
int previousSystemMode = 0;
//Motor variables
int defaultSpeed = 255;
//Adjust these for turning
int motorLeftSpeed = 255, motorRightSpeed = 255;	//Speeds from -255 to +255
long turnFinishTime = -1; //-1 if not in use, positive time otherwise. In milliseconds.


void setup() {
  lcd.begin(16,2);
  
  pinMode(on_off_pin, INPUT);
  pinMode(input1_pin, OUTPUT);
  pinMode(input2_pin, OUTPUT);
  pinMode(input3_pin, OUTPUT);
  pinMode(input4_pin, OUTPUT);
  pinMode(enable12_pin, OUTPUT);
  pinMode(enable34_pin, OUTPUT);
  pinMode(ldr1_pin, INPUT);
  pinMode(ldr2_pin, INPUT);
  pinMode(dip1_pin, INPUT);
  pinMode(dip2_pin, INPUT);
  
  //ANTHONY
}

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void loop() {
  //On/Off switch
  if (digitalRead(on_off_pin) == LOW){
	  systemMode == OFF;
	  analogWrite(enable12_pin, 0);
	  analogWrite(enable34_pin, 0);
	  return;	//skips to next run of loop()
  }
  if (previousSystemMode == OFF && digitalRead(on_off_pin) == HIGH){
	  initialTurning();
  }
  
  //Update LDRs
  ldr1_state = analogRead(ldr1_pin);
  ldr2_state = analogRead(ldr2_pin);
  //Update LCDs
  udsLeft_cm = updateUDS(uds_left_pin)/58.2377;
  udsRight_cm = updateUDS(uds_right_pin)/58.2377;
  udsCenter_cm = updateUDS(uds_center_pin)/58.2377
  //Update LCD with sensor data
  updateLCD();
  
  //NEAR_EDGE mode will not be deactivated until escape maneuver has been performed. Revert to search.
  if (systemMode == NEAR_EDGE || ldr1_state < 700 || ldr2_state < 700){
	  systemMode == NEAR_EDGE;
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
		  motorRightSpeed = defualtSpeed;
	  }
	  else if (udsRight_cm < 300 && udsCenter_cm < 300){	//Turn soft right
		  motorLeftSpeed = defaultSpeed;
		  motorRightSpeed = defaultSpeed - 70;
	  }
	  else if (udsLeft_cm < 300){	//Turn hard left
		  motorLeftSpeed = defaultSpeed - 150;
		  motorRightSpeed = defualtSpeed;
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
  
  //Near-edge escape maneuver
  updateMotorStates();
  
}

void updateLCD(){
  //Erase display
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(8, 0);
  lcd.print(ldr1_state);
  lcd.setCursor(8, 1);
  lcd.print(ldr2_state);
  lcd.setCursor(0,1);
  lcd.print(uds1_cm);
  lcd.setCursor(4,1);
  lcd.print(uds2_cm);
  lcd.setCursor(2,0);
  lcd.print(uds3_cm);
  //Columns 13,14,15 are usable. OFF, SCH, TRK, ATK, EDG
}

//Update UDSs
long updateUDS(int pin){
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delayMicroseconds(2);
	digitalWrite(pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(pin, LOW);
	pinMode(pin, LOW);
	return pulseIn(pin, LOW, 20000);
}

//Motor methods. 
//UNCERTAIN, DO TESTING FIRST
void initiateTurn(int turnDegrees, int turnSpeed){
  //states: turn time has passed, turn time is ongoing, turn time is not in use
}

//UNCERTAIN, DO TESTING FIRST
//Test to make sure directions are accurate. motor 12 should be 
void updateMotorStates(){
  //Right motor
  if (motor12Speed < 0){
    digitalWrite(input1_pin, HIGH);
    digitalWrite(input2_pin, LOW);
  }
  else{
    digitalWrite(input1_pin, LOW);
    digitalWrite(input2_pin, HIGH);
  }
  analogWrite(enable12_pin, motorRightSpeed);
  //Left motor
  if (motor34Speed < 0){
    digitalWrite(input3_pin, HIGH);
    digitalWrite(input4_pin, LOW);
  }
  else{
    digitalWrite(input3_pin, LOW);
    digitalWrite(input4_pin, HIGH);
  }
  analogWrite(enable34_pin, motorLeftSpeed);
}

//Note: there is no use in trying to predict required turn times without testing
//Would need mass of car and power curve of motors to make theoretical prediction, or testing
void initialTurning(){
  defaultSpeed = 0;	//If you don't want to turn on the spot for whatever reason, change this
  int dip1_state = analogRead(dip1_pin);
  int dip2_state = analogRead(dip2_pin);
  if (dip1_state < 500 && dip2_state < 500){
	  return;	//No turning
  }
  else if (dip1_state < 500 && dip2_state > 500){
    //send commands to motor to turn 90 degrees CW (right)
	motorLeftSpeed = defaultSpeed - 100;
	motorRightSpeed = defaultSpeed + 100;
    turnFinishTime = millis() + 1000;
  }
  else if (dip1_state > 500 && dip2_state < 500){
    //send commands to motor to turn 180 degrees CW (right)
	motorLeftSpeed = defaultSpeed - 100;
	motorRightSpeed = defaultSpeed + 100;
	turnFinishTime = millis() + 2000;
  }
  else if (dip1_state > 500 && dip2_state > 500){
    //send commands to motor to turn 90 degrees CCW (left)
	motorLeftSpeed = defaultSpeed + 100;
	motorRightSpeed = defaultSpeed - 100;
	turnFinishTime = millis() + 1000;
  }
  while (millis() < turnFinishTime){
	updateMotorStates();
	delay(50);
  }
}
