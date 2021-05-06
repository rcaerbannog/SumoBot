#include <LiquidCrystal.h>

int on_off_pin=0;
int enable1_pin=1;
int enable2_pin=2;
int enable3_pin=3;
int enable4_pin=4;
int uds_left_pin=5;
int uds_right_pin=6;
int uds_center_pin=7;
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
int ldr1_pin=A0;
int ldr2_pin=A1;
int dip1_pin=A4;
int dip2_pin=A5;
int ldr1_state=0;
int ldr2_state=0;

//System mode
int systemMode = 0; //0 for idle, 1 for search, 2 for tracking, 3 for facing, 4 for near edge
//Motor variables
int defaultSpeed = 255;
int motor12Speed = 255;
int motor34Speed = 255;
//add turn stuff here
long turnFinishTime = -1; //-1 if not in use, positive time otherwise. In milliseconds.

long uds1_cm=0;
long uds2_cm=0;
long uds3_cm=0;

void setup() {
  lcd.begin(16,2);
  
  pinMode(on_off_pin,INPUT);
  pinMode(enable1_pin,OUTPUT);
  pinMode(enable2_pin,OUTPUT);
  pinMode(enable3_pin,OUTPUT);
  pinMode(enable4_pin,OUTPUT);
  pinMode(ldr1_pin, INPUT);
  pinMode(ldr2_pin, INPUT);
  pinMode(dip1_pin,INPUT);
  pinMode(dip2_pin,INPUT);
  
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
  //OLD CODE
  ldr1_state=analogRead(ldr1_pin);
  
  lcd.setCursor(12, 0);
  lcd.print(ldr1_state);
    
  ldr2_state=analogRead(ldr2_pin);
  
  lcd.setCursor(12, 1);
  lcd.print(ldr2_state);
  
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  uds1_cm = 0.01723 * readUltrasonicDistance(7, 7);
  lcd.setCursor(0,1);
  lcd.print(uds1_cm);
  uds2_cm = 0.01723 * readUltrasonicDistance(8, 8);
  lcd.setCursor(4,1);
  lcd.print(uds2_cm);
  uds3_cm = 0.01723 * readUltrasonicDistance(9, 9);
  lcd.setCursor(2,0);
  lcd.print(uds3_cm);
  delay(300);
  
  
  //ANTHONY
  
  //ALEXANDER
  
}

//OLD CODE

//ANTHONY

//ALEXANDER

//Motor methods. 
//UNCERTAIN, DO TESTING FIRST
void updateTurning(){
  //states: turn time has passed, turn time is ongoing, turn time is not in use
}

//UNCERTAIN, DO TESTING FIRST
//Test to make sure directions are accurate. motor 12 should be 
void updateMotorStates(){
  if (motor12Speed < 0){
    digitalWrite(input1_pin, HIGH);
    digitalWrite(input2_pin, LOW);
  }
  else{
    digitalWrite(input1_pin, LOW);
    digitalWrite(input2_pin, HIGH);
  }
  analogWrite(enable12_pin, motor12Speed);
  if (motor34Speed < 0){
    digitalWrite(input3_pin, HIGH);
    digitalWrite(input4_pin, LOW);
  }
  else{
    digitalWrite(input3_pin, LOW);
    digitalWrite(input4_pin, HIGH);
  }
  analogWrite(enable34_pin, motor34Speed);
}

//UNFINISHED, wait until motor control is implemented
void initialTurning(){
  int dip1_state = analogRead(dip1_pin);
  int dip2_state = analogRead(dip2_pin);
  if (dip1_state < 500 && dip2_state > 500){
    //send commands to motor to turn 90 degrees CCW (left)
    
  }
  else if (dip1_state > 500 && dip2_state < 500){
    //send commands to motor to turn 180 degrees
  }
  else if (dip1_state > 500 && dip2_state > 500){
    //send commands to motor to turn 90 degrees CW (right)
  }
  //Otherwise, dip1_state < 500 && dip2_state < 500, in which case do nothing (turn 0 degrees)
}
