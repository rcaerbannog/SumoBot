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

void loop() {
  //OLD CODE
  ldr1_state=analogRead(ldr1_pin);
  
  lcd.setCursor(12, 0);
  lcd.print(ldr1_state);
    
  ldr2_state=analogRead(ldr2_pin);
  
  lcd.setCursor(12, 1);
  lcd.print(ldr2_state);
  
  
  //ANTHONY
  
  //ALEXANDER
  
}

//OLD CODE

//ANTHONY

//ALEXANDER
  
