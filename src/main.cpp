#include <Arduino.h>
#include "Config_Pins.h"
#include "Config_Setup.h"
#include <SPI.h>




/**
General Notes

When controlling stepper motor direction 0 is towards home
*/



//Game Phase Progression Vars
byte G_Phase = 0;
bool G_Trip = false;

//Timer variables
bool G_Timing = false;
byte G_Time_Out = false;



long Z_Height = 200;
long G_Z_Count = 0;

bool dirX = false;
int lastButtonState = LOW;




void setup() {
  Serial.begin(9600);

  //Stepper X Pin Setup
  digitalWrite(X_ENABLE_PIN,HIGH);
  pinMode(X_ENABLE_PIN ,OUTPUT);
  pinMode(X_STEP_PIN,OUTPUT);
  pinMode(X_DIR_PIN,OUTPUT);
  //Stepper Y Pin Setup
  digitalWrite(Y_ENABLE_PIN,HIGH);
  pinMode(Y_ENABLE_PIN ,OUTPUT);
  pinMode(Y_STEP_PIN,OUTPUT);
  pinMode(Y_DIR_PIN,OUTPUT);
  //Stepper Z Pin Setup
  digitalWrite(Z_ENABLE_PIN,HIGH);
  pinMode(Z_ENABLE_PIN ,OUTPUT);
  pinMode(Z_STEP_PIN,OUTPUT);
  pinMode(Z_DIR_PIN,OUTPUT);

  //Limit switch setup
  pinMode(X_MIN_PIN, INPUT_PULLUP);
  pinMode(X_MAX_PIN, INPUT_PULLUP);
  pinMode(Y_MIN_PIN, INPUT_PULLUP);
  pinMode(Y_MAX_PIN, INPUT_PULLUP);
  pinMode(Z_MIN_PIN, INPUT_PULLUP);
  pinMode(Z_MAX_PIN, INPUT_PULLUP);



  //Built in LED
  pinMode(LED_BUILTIN, OUTPUT);

  //Aux 2 Pin Setup
  pinMode(B_PIN, INPUT);
  pinMode(C_PIN, INPUT);

  /** Re-Enable to aactivate cooling fan.
  pinMode(Main_Fan_Pin, OUTPUT);
  digitalWrite(Main_Fan_Pin,LOW);
    */


  //TIMER INTERUPTS
  cli();//stop interrupts
  //set timer4 interrupt at 1Hz
   TCCR4A = 0;// set entire TCCR1A register to 0
   TCCR4B = 0;// same for TCCR1B
   TCNT4  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR4A = 15624/1;// = (16*10^6) / (1*1024) - 1 (must be <65536)
   // turn on CTC mode
   TCCR4B |= (1 << WGM12);
   // Set CS12 and CS10 bits for 1024 prescaler
   TCCR4B |= (1 << CS12) | (1 << CS10);
   // enable timer compare interrupt
   TIMSK4 |= (1 << OCIE4A);
  sei();//allow interrupts

}

ISR(TIMER4_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  //Serial.println(millis()/1000); //Prints time left
  if (G_Timing == true){ //if timer is active
    if (G_Start_Time < 0){ //if timer reached 0 then end timer and flag it
      G_Timing = false;
      Serial.println("TIME OUT");
      G_Time_Out = true;
    } else { // if not at 0, count down and update LCD time
      Serial.println(G_Start_Time);
      G_Start_Time -= 1;
    }
  }
} //ISR TIMER END



boolean MoveAxis_OLD(boolean S_Direct){
    digitalWrite(Z_DIR_PIN, S_Direct); // set direction, HIGH for clockwise, LOW for anticlockwise
    digitalWrite(Z_ENABLE_PIN,LOW); //enables stepper if its not already enabled
    digitalWrite(Z_STEP_PIN,HIGH);
    delayMicroseconds(X_Speed);
    digitalWrite(Z_STEP_PIN,LOW);
    delayMicroseconds(X_Speed); //max 300
    return true;
}


boolean MoveAxis(char S_Motor, boolean S_Direct, int S_Speed){
  byte S_STEP_PIN;
  byte S_DIR_PIN;
  byte S_ENABLE_PIN;
  byte S_LIMIT_PIN;

  switch (S_Motor) {
    case 'X': //Set up to move motor X
        S_STEP_PIN = X_STEP_PIN;
        S_DIR_PIN = X_DIR_PIN;
        S_ENABLE_PIN = X_ENABLE_PIN;
        if(X_Direction == 1){ //corrects stepper direction when required
            S_Direct = !S_Direct;
        }
        if(S_Direct == 0){ //this block sets the limit switch to kill the command
          S_LIMIT_PIN = X_MIN_PIN ;
        } else {
          S_LIMIT_PIN = X_MAX_PIN ;
        }
      break;
    case 'Y': //Set up to move motor Y
        S_STEP_PIN = Y_STEP_PIN;
        S_DIR_PIN = Y_DIR_PIN;
        S_ENABLE_PIN = Y_ENABLE_PIN;
        if(Y_Direction == 1){//corrects stepper direction when required
            S_Direct = !S_Direct;
        }
        if(S_Direct == 0){ //this block sets the limit switch to kill the command
          S_LIMIT_PIN = Y_MIN_PIN ;
        } else {
          S_LIMIT_PIN = Y_MAX_PIN ;
        }
      break;
    case 'Z': //Set up to move motor Y
          S_STEP_PIN = Z_STEP_PIN;
          S_DIR_PIN = Z_DIR_PIN;
          S_ENABLE_PIN = Z_ENABLE_PIN;
          if(Z_Direction == 1){//corrects stepper direction when required
              S_Direct = !S_Direct;
          }
          if(S_Direct == 0){ //this block sets the limit switch to kill the command
            S_LIMIT_PIN = Z_MIN_PIN ;
          } else {
            S_LIMIT_PIN = Z_MAX_PIN ;
          }
        break;
    }


    if (digitalRead(S_LIMIT_PIN) == 0){
      return false;
    }
    digitalWrite(S_DIR_PIN, S_Direct); // set direction, HIGH for clockwise, LOW for anticlockwise
    digitalWrite(S_ENABLE_PIN,LOW); //enables stepper if its not already enabled
    digitalWrite(S_STEP_PIN,HIGH);
    delayMicroseconds(S_Speed);
    digitalWrite(S_STEP_PIN,LOW);
    delayMicroseconds(S_Speed); //max 300
    return true;
}




void Manouver(char S_Motor, boolean S_Direct, int S_Speed, byte S_Trigger){
  while ((digitalRead(Y_MIN_PIN) == 1) && (digitalRead(S_Trigger) == 1)){
    if (MoveAxis(S_Motor, S_Direct, S_Speed) == false){
      return;
    }
  }
}


void Home(char S_Motor, boolean S_Direct, int S_Speed, byte S_Trigger){
  while (digitalRead(Y_MIN_PIN) == 1){
    MoveAxis(S_Motor, S_Direct, S_Speed);
  }
}

void loop() {



  // Wait for B_Pin to be pressed
  while (digitalRead(B_PIN) == 0){
    delay(20);
  }

  //Trigger timer and move untill B_Pin rerleased
  if (digitalRead(B_PIN) == 1){
    G_Start_Time = 10;
    G_Timing = true;
    Manouver('Y', 1, X_Speed, B_PIN);
  }

  while (digitalRead(C_PIN) == 0){
    delay(20);
  }

  if (digitalRead(C_PIN) == 1){
    Manouver('Z', 1, Default_Speed, C_PIN);
  }

  delay(600);
  Home('Y', 0, X_Speed, B_PIN); //Home Y
  delay(500);
  Home('Z', 0, X_Speed, B_PIN); //Home Z

  G_Timing = false;

// Serial.print(digitalRead(Y_MIN_PIN));
// Serial.print(digitalRead(Y_MAX_PIN));
// Serial.print(digitalRead(Z_MIN_PIN));
// Serial.println(digitalRead(Z_MAX_PIN));

}
