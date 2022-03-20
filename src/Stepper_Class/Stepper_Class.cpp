#include <Arduino.h>
#include "Stepper_Class.h"



Stepper::Stepper(byte StepPin, byte DirPin, byte EnPin, byte EStopMin, byte EStopMax, bool ReverseMotor){
  //Takes in all the arguments
  Step_Pin = StepPin;
  Dir_Pin = DirPin;
  En_Pin = EnPin;
  EStopHome = EStopMin;
  EStopAway = EStopMax;
  Reverse = ReverseMotor;
  //Initialize MCU Pins
  digitalWrite(En_Pin, HIGH);
  pinMode(En_Pin, OUTPUT);
  pinMode(Step_Pin, OUTPUT);
  pinMode(Dir_Pin, OUTPUT);
  pinMode(EStopHome, INPUT_PULLUP);
  pinMode(EStopAway, INPUT_PULLUP);
}

void Stepper::SetReverse(bool Rev){
  Reverse = Rev;
}

void Stepper::StepDir(bool newDir){
  Direction = newDir;
  if (Reverse == 0){
    digitalWrite(Dir_Pin, Direction);
  } else {
    digitalWrite(Dir_Pin, !Direction);
  }
}

byte Stepper::GetEStop(){
  if (Direction == 0) return EStopHome;
  return EStopAway;
}

void Stepper::StepEnable(bool enab){
  digitalWrite(En_Pin,!enab);
}

void Stepper::Drive(bool denable){
  StepDrive = denable;
}
