#ifndef DANOGRAB_PINS_SIRDAN //Header Guard
  #define DANOGRAB_PINS_SIRDAN

  //stepper motor driver pin allocation

  //X Stepepr pins
  #define X_STEP_PIN                          54
  #define X_DIR_PIN                           55
  #define X_ENABLE_PIN                        38

  //Y Stepper pins
  #define Y_STEP_PIN                          60
  #define Y_DIR_PIN                           61
  #define Y_ENABLE_PIN                        56

  //Z Stepper pins
  #define Z_STEP_PIN                          46
  #define Z_DIR_PIN                           48
  #define Z_ENABLE_PIN                        62

  //E0 Stepper pins
  #define E0_STEP_PIN                         26
  #define E0_DIR_PIN                          28
  #define E0_ENABLE_PIN                       24

  //E1 Stepper pins
  #define E1_STEP_PIN                         36
  #define E1_DIR_PIN                          34
  #define E1_ENABLE_PIN                       30


  //Limit Switches Pin allocation
  #define X_MIN_PIN                          3
  #define X_MAX_PIN                          2
  #define Y_MIN_PIN                         14
  #define Y_MAX_PIN                         15
  #define Z_MIN_PIN                         18
  #define Z_MAX_PIN                         19



  /**
  AUX-2 Pins
  GND   PIN A   PIN B   PIN C   Pin D
  12v   PIN E   PIN F   PIN G   Pin H
   */
  #define AUX2_B_PIN                               40
  #define AUX2_C_PIN                               42
  #define AUX2_G_PIN                               44
  #define AUX2_F_PIN                               A10

  // User Buttons
  #define F_BUTTON                        AUX2_B_PIN
  #define R_BUTTON                        AUX2_C_PIN
  #define S_BUTTON                        AUX2_G_PIN
  #define B_BUTTON                        AUX2_F_PIN

  //
  // AUX3 : GND D52 D50 5V
  //        NC  D53 D51 D49

  #define AUX3_03_PIN                           53
  #define AUX3_04_PIN                           52
  #define AUX3_05_PIN                           51
  #define AUX3_06_PIN                           50
  #define AUX3_07_PIN                           49

  //
  // AUX4 : D16 D17 D23 D25 D27 D29 D31 D33 D35 D37 D39 D41 D43 D45 D47 D32 GND 5V
  //
  #define AUX4_03_PIN                           32
  #define AUX4_04_PIN                           47
  #define AUX4_05_PIN                           45
  #define AUX4_06_PIN                           43
  #define AUX4_07_PIN                           41
  #define AUX4_08_PIN                           39
  #define AUX4_09_PIN                           37
  #define AUX4_10_PIN                           35
  #define AUX4_11_PIN                           33
  #define AUX4_12_PIN                           31
  #define AUX4_13_PIN                           29
  #define AUX4_14_PIN                           27
  #define AUX4_15_PIN                           25
  #define AUX4_16_PIN                           23
  #define AUX4_17_PIN                           17
  #define AUX4_18_PIN                           16

  #define EXP1_03_PIN                           29 //LCD7
  #define EXP1_04_PIN                           27 //LCD6
  #define EXP1_05_PIN                           25 //LCD5
  #define EXP1_06_PIN                           23 //LCD4 E(SLCK)
  #define EXP1_07_PIN                           16 //LCDRS (CS)
  #define EXP1_08_PIN                           17 //LCDE RW(SID)
  #define EXP1_09_PIN                           35 //BTN ENC
  #define EXP1_10_PIN                           37 //Beeper

  #define EXP2_03_PIN                           41 //Reset
  #define EXP2_04_PIN                           49 //SD_DET
  #define EXP2_05_PIN                           51 //PB2 MOSI
  #define EXP2_06_PIN                           33 //BTN_EN1
  #define EXP2_07_PIN                           53 //SD_CSEL
  #define EXP2_08_PIN                           31 //BTN_EN2
  #define EXP2_09_PIN                           52 //PB1 (SCK)
  #define EXP2_10_PIN                           50 //PB3 (MISO)

  // LCD Display output pins
  #define LCD_PINS_RS          EXP1_07_PIN //LCDRS (CS)
  #define LCD_PINS_ENABLE      EXP1_08_PIN //LCDE RW(SID)
  #define LCD_PINS_D4          EXP1_06_PIN //LCD4 E(SLCK)
  #define LCD_PINS_D5          EXP1_05_PIN //LCD5
  #define LCD_PINS_D6          EXP1_04_PIN //LCD6
  #define LCD_PINS_D7          EXP1_03_PIN //LCD7
  #define BEEPER_PIN           EXP1_10_PIN //Beeper

  // LCD Display input pins
  #define BTN_ENC              EXP1_09_PIN
  #define BTN_EN1              EXP2_08_PIN
  #define BTN_EN2              EXP2_06_PIN
  #define SD_DETECT_PIN        EXP2_04_PIN
  #define KILL_PIN             EXP2_03_PIN

  //Pin 9 Turns on D9 12v output
  #define Main_Fan_Pin                        9



#endif
