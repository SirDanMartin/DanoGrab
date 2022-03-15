/**
--------------------
||Machine Settings||
--------------------
*/

//Movement Speeds (max speed 500, lower is faster)
#define X_Speed                             300
#define Y_Speed                             300
#define Z_Speed                             300
#define Default_Speed                       300
#define XY_Speed                            300
#define XY_Home_Speed                       300
#define Z_Home_Speed                        300

//Motor Direction - Changing the value to 1 to reverses motor directions
#define X_Direction                         0
#define Y_Direction                         0
#define Z_Direction                         1

#define Steps_Per_mm                        200

/**
-----------------
||Game Settings||
-----------------
*/

int G_Start_Time = 15;
int Crane_Drop_Distance = 5; //Maximum distance to drop the crane. (Used if the limit switch does not trigger.)
