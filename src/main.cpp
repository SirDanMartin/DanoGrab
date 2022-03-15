#include <Arduino.h>
#include "Config_Pins.h"
#include "Config_Setup.h"
#include <SPI.h>
#include <LiquidCrystal.h>



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
int G_Remaining_Time;

//Set up for current screen
LiquidCrystal lcd(27, 25, 16, 35, 23 , 17);

volatile bool XYSwitch = false;
volatile bool ZSwitch = false;
bool X_Drive = false;
bool Y_Drive = false;
bool Z_Drive = false;

void setup() {
  //Serial.begin(9600);

  //Sets up LCD
  analogWrite(A12, 255);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("CraneGame");
  lcd.setCursor(0,1);
  lcd.print("Press Start");

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
  pinMode(B_PIN, INPUT_PULLUP);
  pinMode(C_PIN, INPUT_PULLUP);
  pinMode(G_PIN, INPUT_PULLUP);

  /** Re-Enable to aactivate cooling fan.
  pinMode(Main_Fan_Pin, OUTPUT);
  digitalWrite(Main_Fan_Pin,HIGH);
  */


  //TIMER INTERUPTS
  cli();//stop interrupts
  //set timer4 interrupt at 1Hz
   TCCR2A = 0;// set entire TCCR1A register to 0
   TCCR2B = 0;// same for TCCR1B
   TCNT2  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR2A = 15624/1;// = (16*10^6) / (1*1024) - 1 (must be <65536)
   // turn on CTC mode
   TCCR2B |= (1 << WGM12);
   // Set CS12 and CS10 bits for 1024 prescaler
   TCCR2B |= (1 << CS12) | (1 << CS10);
   // enable timer compare interrupt
   TIMSK2 |= (1 << OCIE2A);


   TCCR4A = 0;// set entire TCCR1A register to 0
   TCCR4B = 0;// same for TCCR1B
   TCNT4  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR4A = 15624/XY_Speed;// = (16*10^6) / (1*1024) - 1 (must be <65536)
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


  digitalWrite(LED_BUILTIN, LOW);






}

ISR(TIMER2_COMPA_vect){
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  //Serial.println(millis()/1000); //Prints time left
  sei(); //enbale nested interupts
  if (G_Timing == true){ //if timer is active
    if (G_Remaining_Time < 0){ //if timer reached 0 then end timer and flag it
      G_Timing = false;
      lcd.setCursor(13,0);
      lcd.print("  ");
      lcd.setCursor(0,1);
      lcd.print("TIME OUT                ");
      G_Time_Out = true;
    } else { // if not at 0, count down and update LCD time
      //Serial.println(G_Remaining_Time);
      lcd.setCursor(13,0);
      lcd.print("  ");
      lcd.setCursor(13,0);
      lcd.print(G_Remaining_Time);
      G_Remaining_Time -= 1;
    }
  }
} //ISR TIMER END

ISR(TIMER4_COMPA_vect){ //X Timer Interupt

  //if limit swith open then send step signal low else

  if (X_Drive == true){
    XYSwitch = !XYSwitch;
    digitalWrite(Z_STEP_PIN,XYSwitch); // if end stop open then send step
  }

}


ISR(TIMER5_COMPA_vect){ //Z Timer Interupt
  if (Z_Drive == true){
    ZSwitch = !ZSwitch;
    digitalWrite(Z_STEP_PIN,ZSwitch); // if end stop open then send step
  }
}

void Stepers_Off(){
  digitalWrite(X_ENABLE_PIN,HIGH);
  digitalWrite(Y_ENABLE_PIN,HIGH);
  digitalWrite(Z_ENABLE_PIN,HIGH);
}







bool Step_XY(bool X_on, bool X_dir, bool Y_on, bool Y_dir, int S_Speed){
	int X_Stop;
	int Y_Stop;

	digitalWrite(X_DIR_PIN, X_dir); // set direction, LOW/0/false = Towards Home, HIGH/1/true = Away from home.
	digitalWrite(X_ENABLE_PIN,!X_on); //enables stepper if its not already enabled
	if(X_dir == 0){ //this block sets the limit switch which will prevent movement if triggered
			X_Stop = X_MIN_PIN ;
		} else {
			X_Stop = X_MAX_PIN ;
		}

	digitalWrite(Y_DIR_PIN, Y_dir); // set direction, LOW/0/false = Towards Home, HIGH/1/true = Away from home.
	digitalWrite(Y_ENABLE_PIN,!Y_on); //enables stepper if its not already enabled
	if(Y_dir == 0){ //this block sets the limit switch which will prevent movement if triggered
			Y_Stop = Y_MIN_PIN ;
		} else {
			Y_Stop = Y_MAX_PIN ;
		}

	if ((digitalRead(X_Stop) + digitalRead(Y_Stop)) == 0) return false; //if all limit switches are triggered then return false


	if (digitalRead(X_Stop) == 1) digitalWrite(X_STEP_PIN,HIGH); // if end stop open then send step
	if (digitalRead(Y_Stop) == 1) digitalWrite(Y_STEP_PIN,HIGH); // if end stop open then send step
  delayMicroseconds(S_Speed);
  digitalWrite(X_STEP_PIN,LOW);
	digitalWrite(Y_STEP_PIN,LOW);
  delayMicroseconds(S_Speed); //max 300
	return true;
}




bool Step_Z(bool Z_dir, int S_Speed){
	int Z_Stop;

	digitalWrite(Z_DIR_PIN, Z_dir); // set direction, LOW/0/false = Towards Home, HIGH/1/true = Away from home.
	digitalWrite(Z_ENABLE_PIN,LOW); //enables stepper if its not already enabled
	if(Z_dir == 0){ //this block sets the limit switch which will prevent movement if triggered
			Z_Stop = Z_MIN_PIN ;
		} else {
			Z_Stop = Z_MAX_PIN ;
		}

	if (digitalRead(Z_Stop) == 0) return false; //if all limit switches are triggered then return false

	if (digitalRead(Z_Stop) == 1) digitalWrite(Z_STEP_PIN,HIGH); // if end stop open then send step
  delayMicroseconds(S_Speed);
	digitalWrite(Z_STEP_PIN,LOW);
  delayMicroseconds(S_Speed); //max 300

	return true;
}




void Move_Single(char S_Axis, bool S_Dir, int S_Trigger, int S_Speed = XY_Speed){
	int S_Stop;
	bool X_On = false;
	bool Y_On = false;

  //Turns of X axis and sers appropriate end stop for the direction of travel
	if (S_Axis == 'X'){
		X_On = true;
		if (S_Dir == 0){
			S_Stop = X_MIN_PIN;
		} else {
			S_Stop = X_MAX_PIN;
		}
	}

  //Turns of Y axis and sers appropriate end stop for the direction of travel
  if (S_Axis == 'Y'){
		Y_On = true;
		if (S_Dir == 0){
			S_Stop = Y_MIN_PIN;
		} else {
			S_Stop = Y_MAX_PIN;
		}
	}
	while ((digitalRead(S_Trigger) == 0) && (digitalRead(S_Stop) == 1) && (G_Time_Out == false)){
		if (Step_XY(X_On, S_Dir, Y_On, S_Dir, S_Speed) == false){
      return;
    }
	}

	Stepers_Off();
}


void Lower_Z(){
  int i;
  long L_Steps;
  L_Steps = (Steps_Per_mm * Crane_Drop_Distance) + 1;
  lcd.setCursor(0,1);
  lcd.print("Lower Z      ");
  for (i = 1; i < L_Steps; ++i)
  {
		if (Step_Z(1, Z_Speed) == false){
      lcd.setCursor(0,1);
      lcd.print("Return False       ");
      return;
    }
  }
}

bool Home_Z(int S_Speed = Z_Home_Speed){

	while (digitalRead(Z_MIN_PIN) == 1){
		Step_Z(0, Z_Home_Speed);
	}
	return true;
}


bool Home_XY(bool X_On, bool Y_On, int S_Speed = XY_Home_Speed){
  //this code might need ot be re-enabled to only home when the z is already home. Removed for testing ease
	//while (((digitalRead(X_MIN_PIN) == 1) || (digitalRead(Y_MIN_PIN) == 1)) && (digitalRead(Z_MIN_PIN) == 0)){
  while ((digitalRead(X_MIN_PIN) == 1) || (digitalRead(Y_MIN_PIN) == 1)){
		Step_XY(X_On, 0, Y_On, 0, S_Speed);
	}
	Stepers_Off();

	if (digitalRead(Z_MIN_PIN) == 1) return false;
	return true;
}






void loop() {

  lcd.setCursor(0,1);
  lcd.print("Press Start           ");


  // Wait for B_Pin to be pressed *Simulate Coin Entered
  while (digitalRead(G_PIN) == 1){
    G_Remaining_Time = G_Start_Time;
    G_Time_Out = false;
  }

  digitalWrite(LED_BUILTIN, HIGH);
  G_Timing = true;//start timer

  lcd.setCursor(0,1);
  lcd.print("Start          ");





  lcd.setCursor(0,1);
  lcd.print("Press forward         ");
  // Wait for B_Pin to be pressed
  while ((digitalRead(B_PIN) == 1) && (G_Time_Out == false)){
    delay(20);
  }

  //move untill B_Pin rerleased
  if ((digitalRead(B_PIN) == 0) && (G_Time_Out == false)){
    Move_Single('X',1,B_PIN);
  }




  lcd.setCursor(0,1);
  lcd.print("Press right       ");
  // Wait for C_Pin to be pressed
  while ((digitalRead(C_PIN) == 1) && (G_Time_Out == false)){
    delay(20);
  }

  //move untill C_Pin rerleased
  if ((digitalRead(C_PIN) == 0) && (G_Time_Out == false)){
    Move_Single('Y',1,C_PIN);


  }



  lcd.setCursor(0,1);
  lcd.print("Press lower       ");
  //wait for G_Pin to be pressed //probavly wouldnt need to wait for user when release but helps with dev
  while ((digitalRead(G_PIN) == 1) && (G_Time_Out == false)){
    delay(20);
  }

  if ((digitalRead(G_PIN) == 0)  && (G_Time_Out == false)){
    Lower_Z();
  }


  //END OF USE CONTROLLED SECTION

  G_Timing = false; //Stops the timer as user has finished input
  G_Time_Out = false;
  lcd.setCursor(13,0);
  lcd.print("  ");


  lcd.setCursor(0,1);
  lcd.print("Returning home      ");

  delay(600);
  Home_Z();
  delay(500);
  Home_XY(1,1);
  delay(500);
  //release claw

  Serial.println("END!");
  digitalWrite(LED_BUILTIN, LOW);
  Stepers_Off();


}
