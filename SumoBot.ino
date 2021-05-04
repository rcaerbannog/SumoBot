#include <LiquidCrystal.h>

int on_off_pin=0;
int enable1_pin=1;
int enable2_pin=2;
int enable3_pin=3;
int enable4_pin=4
int uds_left_pin=5;
int uds_right_pin=6;
int uds_center_pin=7;
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
int ldr1_pin=A0;
int ldr2_pin=A1;
int dip1_pin=A4;
int dip2_pin=A5;
int LDR1state=0;
int LDR2state=0;

void setup() {
  lcd.begin(16,2);
  
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
}

void loop() {
  LDR1state=analogRead(LDR1);
  
  lcd.setCursor(12, 0);
  lcd.print(LDR1state);
    
  LDR2state=analogRead(LDR2);
  
  lcd.setCursor(12, 1);
  lcd.print(LDR2state);
}
  
