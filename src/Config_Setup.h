#ifndef CONFIG_SETUP_SIRDAN //Header Guard
  #define CONFIG_SETUP_SIRDAN

  #include <Arduino.h>
  #include "DanoGrab_Pins.h"
  /**
  --------------------
  ||Machine Settings||
  --------------------
  */

  //Movement Speeds (max speed 500, lower is faster)
  #define X_Speed                             5000
  #define Y_Speed                             5000
  #define Z_Speed                             2000
  #define Default_Speed                       300
  #define XY_Speed                            1500
  #define XY_Home_Speed                       300
  #define Z_Home_Speed                        1000

  //Motor Direction - Changing the value to 1 to reverses motor directions
  #define X_Direction                         0
  #define Y_Direction                         0
  #define Z_Direction                         1

  #define Steps_Per_Rev                        1600

  /**
  -----------------
  ||Game Settings||
  -----------------
  */

  int G_Start_Time = 30;
  int Crane_Drop_Distance = 5; //Maximum distance to drop the crane. (Used if the limit switch does not trigger.)
  int G_Stop_Delay = 400; //maximum delay when using random delay stop


  /**
  -----------------
  ||Program Variables||
  -----------------
  */

  //Game Phase Progression Vars
  bool G_Trip = false;

  //Timer variables
  bool G_Timing = false;
  bool G_Time_Out = false;
  int G_Remaining_Time;

  // Timer interupt motor vars
  bool XSwitch = false;
  bool YSwitch = false;
  bool ZSwitch = false;
  bool Stepper_Drive[3] = {false, false, false};


  #endif
