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

  int G_Start_Time = 15;
  int Crane_Drop_Distance = 5; //Maximum distance to drop the crane. (Used if the limit switch does not trigger.)

  int G_Stop_Delay = 400;



  /**
  -----------------
  ||Program Variables||
  -----------------
  */

  //creats arrays to store all motor info neatly
  // 0 = x, 1 = y, 2 = z
  byte STEP_PIN[3] = {X_STEP_PIN, Y_STEP_PIN, Z_STEP_PIN};
  byte DIR_PIN[3] = {X_DIR_PIN, Y_DIR_PIN, Z_DIR_PIN};
  byte ENABLE_PIN[3] = {X_ENABLE_PIN, Y_ENABLE_PIN, Z_ENABLE_PIN};

  byte EStops[3][2] = {
    {X_MIN_PIN, X_MAX_PIN},
    {Y_MIN_PIN, Y_MAX_PIN},
    {Z_MIN_PIN, Z_MAX_PIN}
  };

  bool Motor_Reverse[3] = {X_Direction, Y_Direction, Z_Direction};

  //Game Phase Progression Vars
  bool G_Trip = false;

  //Timer variables
  bool G_Timing = false;
  bool G_Time_Out = false;
  int G_Remaining_Time;

  // Timer interupt motor vars
  volatile bool XSwitch = false;
  volatile bool YSwitch = false;
  volatile bool ZSwitch = false;
  bool Stepper_Drive[3] = {false, false, false};

  int X_EStop;
  int Y_EStop;
  int Z_EStop;

  #endif
