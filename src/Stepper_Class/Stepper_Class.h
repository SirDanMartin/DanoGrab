#ifndef STEPPER_CLASS_SIRDAN //Header Guard
  #define STEPPER_CLASS_SIRDAN
  #include <Arduino.h>

  class Stepper {
  public:
    //Set on decleration
    byte Step_Pin, Dir_Pin, En_Pin;
    byte EStopHome, EStopAway;
    bool Reverse;
    //Not set on decleration
    bool Direction;
    bool StepDrive;
    bool Switch;
  public:
    Stepper(byte StepPin, byte DirPin, byte EnPin, byte EStopMin, byte EStopMax, bool ReverseMotor);
    void SetReverse(bool Rev);
    void StepDir(bool newDir);
    byte GetEStop();
    void StepEnable(bool enab);
    void Drive(bool denable);
  };

#endif
