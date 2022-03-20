//External libraries
#include <Arduino.h>
#include <SPI.h>
//Internal libraries
#include "LCD\LCD_Module.h"
#include "DanoGrab_Pins.h"
#include "Config_Setup.h"


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

class Stepper {
public:
  //Set on decleration
  byte Step_Pin, Dir_Pin, En_Pin;
  byte EStopHome, EStopAway;
  bool Reverse = false;
  //Not set on decleration
  bool Direction = 0;
  bool StepDrive = 0;
  bool Switch = 0;

public:
  Stepper(byte StepPin, byte DirPin, byte EnPin, bool EStopMin, bool EStopMax, bool ReverseMotor){
    Step_Pin = StepPin;
    Dir_Pin = DirPin;
    En_Pin = EnPin;
    EStopHome = EStopMin;
    EStopAway = EStopMax;
    Reverse = ReverseMotor;
  }

  void SetReverse(bool Rev){
    Reverse = Rev;
  }

  void StepDir(bool newDir){
    Direction = newDir;
  }

  byte GetEStop(){
    if (Direction == 0) return EStopHome;
    return EStopAway;
  }

  void StepEnable(bool enab){
    digitalWrite(En_Pin,!enab);
  }

  void Drive(bool denable){
    StepDrive = denable;
  }

};

//sets up the stepper motors into classes
Stepper StepperX(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, X_MIN_PIN, X_MAX_PIN, X_Direction);
Stepper StepperY(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, Y_MIN_PIN, Y_MAX_PIN, Y_Direction);
Stepper StepperZ(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, Z_MIN_PIN, Z_MAX_PIN, Z_Direction);


void setup() {
  Serial.begin(115200);

  Serial.print(StepperX.Step_Pin);


  //LED Setup
  u8g2.begin();
  LCD_IN_COIN("WELCOME", 0);


  //Stepper Pins Setup
  for (int i = 0; i < 3; i++){
    digitalWrite(ENABLE_PIN[i], HIGH);
    pinMode(ENABLE_PIN[i], OUTPUT);
    pinMode(STEP_PIN[i], OUTPUT);
    pinMode(DIR_PIN[i], OUTPUT);
  }

  //EStop Pins Setup
  for (int i = 0; i < 3; i++){
    for (int i2 = 0; i2 < 2; i2++){
      pinMode(EStops[i][i2], INPUT_PULLUP);
    }
  }

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



  /** Re-Enable to aactivate cooling fan.
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

void Stepers_Enable(bool S_State){
  for (int i = 0; i < 3; i++){
    digitalWrite(ENABLE_PIN[i],!S_State);
  }
}


// ISR(TIMER3_COMPA_vect){ //X or 0 Timer Interupt
//   if ((Stepper_Drive[0] == true) && (digitalRead(EStops[0][digitalRead(DIR_PIN[0])])==1)){
//     XSwitch = !XSwitch;
//     digitalWrite(X_STEP_PIN,XSwitch); // if end stop open then send step
//   }
// }


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

bool Correct_Dir(byte S_Axis, bool S_Dir){
  if (Motor_Reverse[S_Axis] == 1) return !S_Dir;
  return S_Dir;
}


void Home_XY(){
  for(int i = 0; i < 2; i++){ //for x and y
    digitalWrite(DIR_PIN[i],Correct_Dir(i,0));     //Set direction to home
    digitalWrite(ENABLE_PIN[i],LOW);    //Enable motor
    Stepper_Drive[i] = true;            //Start Stepping
  }

  while ((digitalRead(EStops[0][0])==1) || (digitalRead(EStops[1][0])==1)){
    // Loop untill button release or estop triggered or timer runs out
    delay(5);
  }

  for(int i = 0; i < 2; i++){ //for x and y
    Stepper_Drive[i] = false;      //Stop Stepping
    digitalWrite(ENABLE_PIN[i],HIGH);   //Disable motor
  }
}





void Move_Single_Class(Stepper &S_Stepper, bool S_Dir, byte S_Hold){
  S_Stepper.StepDir(S_Dir);   //Select Direction
  S_Stepper.StepEnable(1);   //Enable motor
  S_Stepper.Drive(1);           //Start Stepping
  while ((digitalRead(S_Hold)==0) && (digitalRead(S_Stepper.GetEStop())==1) && (G_Time_Out == false)){
    // Loop untill button release or estop triggered or timer runs out
    delay(5);
  }
  delay(random(0, G_Stop_Delay)); //delayed stop to make game unpredicatable
  S_Stepper.StepEnable(0);         //Stop Stepping
}




void Move_Single(byte S_Axis, bool S_Dir, byte S_Hold){
  digitalWrite(DIR_PIN[S_Axis],Correct_Dir(S_Axis,S_Dir));   //Select Direction
  digitalWrite(ENABLE_PIN[S_Axis],LOW);   //Enable motor
  Stepper_Drive[S_Axis] = true;           //Start Stepping
  while ((digitalRead(S_Hold)==0) && (digitalRead(EStops[S_Axis][S_Dir])==1) && (G_Time_Out == false)){
    // Loop untill button release or estop triggered or timer runs out
    delay(5);
  }
  delay(random(0, G_Stop_Delay)); //delayed stop to make game unpredicatable
  Stepper_Drive[S_Axis] = false;        //Stop Stepping
}


void Move_XY_Free(){
  digitalWrite(ENABLE_PIN[0],LOW);                     //Enable X motor
  digitalWrite(ENABLE_PIN[1],LOW);                     //Enable Y motor
  while ((G_Time_Out==0) && (digitalRead(BTN_ENC)==1)){
    // Loop untill button time out or drop button pressed
    if (digitalRead(F_BUTTON)==0){
      digitalWrite(DIR_PIN[0],Correct_Dir(0,1));  //Select Direction
      Stepper_Drive[0] = true;                    //Start Stepping
    } else if (digitalRead(B_BUTTON)==0){
      digitalWrite(DIR_PIN[0],Correct_Dir(0,0));  //Select Direction
      Stepper_Drive[0] = true;                    //Start Stepping
    } else {
      Stepper_Drive[0] = false;              //Stop Stepping
    }

    if (digitalRead(R_BUTTON)==0){
      digitalWrite(DIR_PIN[1],Correct_Dir(1,1));  //Select Direction
      Stepper_Drive[1] = true;                    //Start Stepping
    } else if (digitalRead(S_BUTTON)==0){
      digitalWrite(DIR_PIN[1],Correct_Dir(1,0));  //Select Direction
      Stepper_Drive[1] = true;                    //Start Stepping
    } else {
      Stepper_Drive[1] = false;              //Stop Stepping
    }
    delay(5);
  }
  digitalWrite(ENABLE_PIN[0],HIGH);                     //Enable X motor
  digitalWrite(ENABLE_PIN[1],HIGH);                     //Enable Y motor
  //Choosing not to dissengage steppers here as most cases there
  //will be a crane and homing operaton after a free move.
}

/**
Move Move_Double
while timer still going
  if x pressed and limit open
    move x
  else
    stop x
  if y pressed and limit open
    move y
  else
    stop y


*/



void loop() {

  //wait for coins to come in
  LCD_IN_COIN(" Enter Coins", 0);
  B_Wait(S_BUTTON);
  Timer(true); //Starts Timer
  digitalWrite(LED_BUILTIN, HIGH);

  Stepers_Enable(true);
  //wait for press forward and move forward
  if (B_Wait(F_BUTTON, " Press Forward") == true){
    //Move_Single(0,1,F_BUTTON);
    Move_Single_Class(StepperX, 1, F_BUTTON);
  }

  //wair for press right and move right
  if (B_Wait(R_BUTTON, " Press Right") == true){
    // Move_Single(1,1,R_BUTTON);
    Move_Single_Class(StepperX, 1, F_BUTTON);
  }

  //Free move lets you move left,right,forward,back as much
  //as you like until time runs out or drop button pressed
  // LCD_Command("Free Move");
  // Move_XY_Free();

  // If time_out == true then play buzzer sound and show timout
  Timer(false); //Stops TImer
  digitalWrite(LED_BUILTIN, LOW);
  delay (200);
  LCD_Command("Returning");
  Home_XY();
  Stepers_Enable(false);


  delay(200);

}
