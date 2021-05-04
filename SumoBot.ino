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
int turnState = 0;  //-2 for fast CCW, -1 for normal CCW, 0 for straight, 1 for normal CW, 2 for fast CW
long turnFinishTime = -1; //-1 if not in use, positive time otherwise. In milliseconds.

void setup() {
  lcd.begin(16,2);
  
  pinMode(ldr1_pin, INPUT);
  pinMode(ldr2_pin, INPUT);
  
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

//Motor methods. 
//UNCERTAIN, DO TESTING FIRST
void updateTurning(){
  //states: turn time has passed, turn time is ongoing, turn time is not in use
}

//UNCERTAIN, DO TESTING FIRST
//Note: if system_mode = 3 (near edge), this will override updateTurning(). If system_mode != 3, this will be overwritten by updateTurning
void turnDirection(int td_direction, int td_degrees){
  int td_turnTime = 0;  //find how long I should turn to get desired direction
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
