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
  
