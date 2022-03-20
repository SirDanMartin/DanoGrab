//External libraries
#include <Arduino.h>
#include <SPI.h>

//Internal libraries
#include "LCD\LCD_Module.h"
#include "DanoGrab_Pins.h"
#include "Config_Setup.h"
#include "Stepper_Class\Stepper_Class.h" //Stepper Object Stored here.


/**
General Notes

When controlling stepper motors; direction 0 is towards home

MIN end stops are home. Max end stops are away.

Stepper Motors are controlled using arrays.
Array index numbers represent the axis as shown;
// 0 = x, 1 = y, 2 = z

Pins are all defines in Config_Pins.h
Machine parameters and Game Setting are in Config_Setup.h
*/


//sets up the stepper motors in classes
Stepper StepperX(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, X_MIN_PIN, X_MAX_PIN, X_Direction);
Stepper StepperY(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, Y_MIN_PIN, Y_MAX_PIN, Y_Direction);
Stepper StepperZ(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, Z_MIN_PIN, Z_MAX_PIN, Z_Direction);




void setup() {
  // Serial.begin(115200);

  //LCD Setup
  u8g2.begin();
  LCD_IN_COIN("WELCOME", 0);

  //Built in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //Aux 2 Pin Setup
  pinMode(F_BUTTON, INPUT_PULLUP);
  pinMode(R_BUTTON, INPUT_PULLUP);
  pinMode(S_BUTTON, INPUT_PULLUP);
  pinMode(B_BUTTON, INPUT_PULLUP);

  //Encoder Pin Setup
  pinMode(BTN_ENC, INPUT_PULLUP);


  /** Re-Enable to aactivate board cooling fan.
  pinMode(Main_Fan_Pin, OUTPUT);
  digitalWrite(Main_Fan_Pin,HIGH);
  */


  //TIMER INTERUPTS
  cli();//stop interrupts
  //set timer4 interrupt at 1Hz
   TCCR1A = 0;// set entire TCCR1A register to 0
   TCCR1B = 0;// same for TCCR1B
   TCNT1  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR1A = 15624 / 1;// = (16*10^6) / (1*1024) - 1 (must be <65536)
   // turn on CTC mode
   TCCR1B |= (1 << WGM12);
   // Set CS12 and CS10 bits for 1024 prescaler
   TCCR1B |= (1 << CS12) | (1 << CS10);
   // enable timer compare interrupt
   TIMSK1 |= (1 << OCIE1A);

   TCCR3A = 0;// set entire TCCR1A register to 0
   TCCR3B = 0;// same for TCCR1B
   TCNT3  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR3A = 15624/X_Speed;// = (16*10^6) / (1*1024) - 1 (must be <65536)
   // turn on CTC mode
   TCCR3B |= (1 << WGM12);
   // Set CS12 and CS10 bits for 1024 prescaler
   TCCR3B |= (1 << CS12) | (1 << CS10);
   // enable timer compare interrupt
   TIMSK3 |= (1 << OCIE3A);

   TCCR4A = 0;// set entire TCCR1A register to 0
   TCCR4B = 0;// same for TCCR1B
   TCNT4  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR4A = 15624/Y_Speed;// = (16*10^6) / (1*1024) - 1 (must be <65536)
   // turn on CTC mode
   TCCR4B |= (1 << WGM12);
   // Set CS12 and CS10 bits for 1024 prescaler
   TCCR4B |= (1 << CS12) | (1 << CS10);
   // enable timer compare interrupt
   TIMSK4 |= (1 << OCIE4A);


   TCCR5A = 0;// set entire TCCR1A register to 0
   TCCR5B = 0;// same for TCCR1B
   TCNT5  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR5A = 15624/Z_Speed;// = (16*10^6) / (1*1024) - 1 (must be <65536)
   // turn on CTC mode
   TCCR5B |= (1 << WGM12);
   // Set CS12 and CS10 bits for 1024 prescaler
   TCCR5B |= (1 << CS12) | (1 << CS10);
   // enable timer compare interrupt
   TIMSK5 |= (1 << OCIE5A);
  sei();//allow interrupts



  //StepperX.Enable(0);

} // END SETUP


ISR(TIMER1_COMPA_vect){
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  sei(); //enbale nested interupts
  if (G_Timing == true){ //if timer is active
    if (G_Remaining_Time < 0){ //if timer reached 0 then end timer and flag it
      G_Timing = false;
      G_Time_Out = true;
    } else { // if not at 0, count down and update LCD time
      LCD_Command(String(G_Remaining_Time));
      G_Remaining_Time -= 1;
    }
  }
} //ISR TIMER END

void Timer(boolean T_On){
  G_Remaining_Time = G_Start_Time;
  G_Timing = T_On;
  G_Time_Out = false;
}




ISR(TIMER3_COMPA_vect){ //X or 0 Timer Interupt
  if ((StepperX.StepDrive == true) && (digitalRead(StepperX.GetEStop()) == 1)){
    StepperX.Switch = !StepperX.Switch;
    digitalWrite(StepperX.Step_Pin, StepperX.Switch); // if end stop open then send step
  }
}


ISR(TIMER4_COMPA_vect){ //Y or 1 Timer Interupt
  if ((StepperY.StepDrive == true) && (digitalRead(StepperY.GetEStop()) == 1)){
    StepperY.Switch = !StepperY.Switch;
    digitalWrite(StepperY.Step_Pin, StepperY.Switch); // if end stop open then send step
  }
}

ISR(TIMER5_COMPA_vect){ //Z or 3 Timer Interupt
  if ((StepperZ.StepDrive == true) && (digitalRead(StepperZ.GetEStop()) == 1)){
    StepperZ.Switch = !StepperZ.Switch;
    digitalWrite(StepperZ.Step_Pin, StepperZ.Switch); // if end stop open then send step
  }
}



//turns all steppers on(1) or off(0)
void Stepers_Enable(bool S_State){
  StepperX.StepEnable(S_State);
  StepperY.StepEnable(S_State);
  StepperZ.StepEnable(S_State);
}

//Waits for a specified button to be pressed or timer runs out
// optional message can be sent to inform the user that the Program
// is expecting something
boolean B_Wait(int W_Button, String B_Disp = ""){
  if (B_Disp != "") LCD_Command(B_Disp); //if optional message included then update LCD
  while (digitalRead(W_Button)==1){
    // Loop untill Button released
    if (G_Time_Out == true){ //if timer runs out then end return false
      return false;
    }
    delay(10);
  }
  return true;
}


//Moves both x and y towards home and stop once both home end stops have been triggered
void Home_XY(){
  StepperX.StepDir(0);      //Select Home Direction
  StepperX.StepEnable(1);   //Enable motor
  StepperX.Drive(1);        //Start Stepping
  StepperY.StepDir(0);      //Select HOme Direction
  StepperY.StepEnable(1);   //Enable motor
  StepperY.Drive(1);        //Start Stepping

  while ((digitalRead(StepperX.GetEStop())==1) || (digitalRead(StepperY.GetEStop())==1)){
    // Loop untill button release or estop triggered or timer runs out
    delay(5);
  }
  StepperX.Drive(0);        //Stop Stepping
  StepperY.Drive(0);        //Stop Stepping
}


//Moves a single axis and stops when specified button is release
//Time running out or hitting an end stop will terminate movement
void Move_Single(Stepper &S_Stepper, bool S_Dir, byte S_Hold){
  S_Stepper.StepDir(S_Dir);       //Select Direction
  S_Stepper.StepEnable(1);        //Enable motor
  S_Stepper.Drive(1);             //Start Stepping
  while ((digitalRead(S_Hold)==0) && (digitalRead(S_Stepper.GetEStop())==1) && (G_Time_Out == false)){
    // Loop untill button release or estop triggered or timer runs out
    delay(5);
  }
  delay(random(0, G_Stop_Delay)); //delayed stop to make game unpredicatable
  S_Stepper.Drive(0);             //Stop Stepping
}


//This routine allows unlimited movement in all directions with as many key
// presses as you like. Movement ends when time runs out or drop button pressed.
void Move_XY_Free(){
  StepperX.StepEnable(1);             //Start Stepping X
  StepperY.StepEnable(1);             //Start Stepping Y
  while ((G_Time_Out==0) && (digitalRead(BTN_ENC)==1)){
    // Loop untill button time out or drop button pressed
    if (digitalRead(F_BUTTON)==0){
      StepperX.StepDir(1);            //Select Direction
      StepperX.Drive(1);              //Start Stepping
    } else if (digitalRead(B_BUTTON)==0){
      StepperX.StepDir(0);            //Select Direction
      StepperX.Drive(1);              //Start Stepping
    } else {
      StepperX.Drive(0);              //Stop Stepping
    }

    if (digitalRead(R_BUTTON)==0){
      StepperY.StepDir(1);            //Select Direction
      StepperY.Drive(1);              //Start Stepping                   //Start Stepping
    } else if (digitalRead(S_BUTTON)==0){
      StepperY.StepDir(0);            //Select Direction
      StepperY.Drive(1);              //Start Stepping
    } else {
      StepperY.Drive(0);              //Stop Stepping
    }
    delay(5);
  }
  StepperX.StepEnable(0);             //Stop X Stepping
  StepperY.StepEnable(0);             //Stop Y Stepping
  //Choosing not to dissengage steppers here as most cases there
  //will be a crane and homing operaton after a free move.
}




void loop() {

  //wait for coins to come in


  LCD_IN_COIN("Enter Coins", 0);
  B_Wait(S_BUTTON);
  Timer(true); //Starts Timer
  digitalWrite(LED_BUILTIN, HIGH);

  Stepers_Enable(true);

  //wait for press forward and move forward
  if (B_Wait(F_BUTTON, " Press Forward") == true){
    Move_Single(StepperX, 1, F_BUTTON);
  }

    //wair for press right and move right
  if (B_Wait(R_BUTTON, " Press Right") == true){
    Move_Single(StepperY, 1, R_BUTTON);
  }

  //Free move lets you move left,right,forward,back as much
  //as you like until time runs out or drop button pressed
  LCD_Command("Free Move");
  Move_XY_Free();

  // If time_out == true then play buzzer sound and show timout
  Timer(false); //Stops TImer-
  digitalWrite(LED_BUILTIN, LOW);
  delay (200);
  LCD_Command("Returning");
  Home_XY();
  Stepers_Enable(false);


  delay(200);

}
