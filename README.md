# DanoGrab
 Arcade grabber/ufo machine built using an Arduino Mega and a Ramps board
 
 
 
 some draft code for review;
 
 ```
 Void Step(bool X_on, bool X_dir, bool Y_on, bool Y_dir, bool Z_on, bool Z_dir, ){
	int X_Stop;
	int Y_Stop;
	int Z_Stop;
	
	digitalWrite(X_DIR_PIN, X_dir); // set direction, HIGH for clockwise, LOW for anticlockwise
	digitalWrite(X_ENABLE_PIN,!X_on); //enables stepper if its not already enabled
	if(X_dir == 0){ //this block sets the limit switch which will prevent movement if triggered
			X_Stop = X_MIN_PIN ;
		} else {
			X_Stop = X_MAX_PIN ;
		}

	digitalWrite(Y_DIR_PIN, X_dir); // set direction, HIGH for clockwise, LOW for anticlockwise
	digitalWrite(Y_ENABLE_PIN,!X_on); //enables stepper if its not already enabled
	if(Y_dir == 0){ //this block sets the limit switch which will prevent movement if triggered
			Y_Stop = Y_MIN_PIN ;
		} else {
			Y_Stop = Y_MAX_PIN ;
		}
		
	digitalWrite(Z_DIR_PIN, X_dir); // set direction, HIGH for clockwise, LOW for anticlockwise
	digitalWrite(Z_ENABLE_PIN,!X_on); //enables stepper if its not already enabled
	if(Z_dir == 0){ //this block sets the limit switch which will prevent movement if triggered
			Z_Stop = Z_MIN_PIN ;
		} else {
			Z_Stop = Z_MAX_PIN ;
		}
		

	if (digitalRead(X_Stop) == 1) digitalWrite(X_STEP_PIN,HIGH);
	if (digitalRead(Y_Stop) == 1) digitalWrite(Y_STEP_PIN,HIGH);
	if (digitalRead(Z_Stop) == 1) digitalWrite(Z_STEP_PIN,HIGH);
  delayMicroseconds(S_Speed);
  digitalWrite(X_STEP_PIN,LOW);
	digitalWrite(Y_STEP_PIN,LOW);
	digitalWrite(Z_STEP_PIN,LOW);
  delayMicroseconds(S_Speed); //max 300
}
```
