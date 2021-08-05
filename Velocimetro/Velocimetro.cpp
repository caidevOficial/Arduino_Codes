/*
 * MIT License
 * 
 * Copyright (c) 2021 [FacuFalcone]
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Servo.h>

Servo needle;
#define SERVO 9

//BUTTONS
#define NOBOTON 0
#define GEARBOX_UP 1
#define GEARBOX_DOWN 2
#define RESTART 3

// LEDS
#define LED_R 10
#define LED_Y 11
#define LED_G 12

int millisBefore = 0; // To control the led's blink
int flagButton = NOBOTON; // Antirebote
int gear = 0;
int flagGear = 0; // to know if i can change the gear.
int actualSpeed = 0;
int maximunSpeed[7] = {0, 30, 60, 90, 120, 150, 180};
int acceleration = 1; // this value could be added or subtracted to the actual speed.
// DISPLAY LEDS -> CAR'S GEARS
int carGearBox[7][7] = {
  //A     B     C     D     E     F     G
  {HIGH ,HIGH ,HIGH ,HIGH ,HIGH ,HIGH ,LOW},  // 0
  {LOW  ,HIGH ,HIGH ,LOW  ,LOW  ,LOW  ,LOW},  // 1
  {HIGH ,HIGH ,LOW  ,HIGH ,HIGH ,LOW  ,HIGH}, // 2
  {HIGH ,HIGH ,HIGH ,HIGH ,LOW  ,LOW  ,HIGH}, // 3
  {LOW  ,HIGH ,HIGH ,LOW  ,LOW  ,HIGH ,HIGH}, // 4
  {HIGH ,LOW  ,HIGH ,HIGH ,LOW  ,HIGH ,HIGH}, // 5
  {HIGH ,LOW  ,HIGH ,HIGH ,HIGH ,HIGH ,HIGH}  // 6
};

/**
 * @brief  Configs the initial state of the program..
 * @note   .
 */
void setup(){
  needle.attach(SERVO,500,2500);
  for(int i=2;i<=11;i++){
     pinMode(i, OUTPUT);
  }
  // prints 0 in the display as a initial state.
  showGearBox(0); 
  moveNeedle(gear);
  Serial.begin(9600);
}

/**
 * @brief  Main lop of the program..
 * @note   
 */
void loop(){
  int buttonNow = readButton();
  accelerate(gear);
  if(buttonNow != NOBOTON && flagButton == NOBOTON){
    if(buttonNow != RESTART){
      gearBoxControl(buttonNow);
    }
    else{
      restartProgram();
    }
    flagButton = buttonNow;
  }
  if(buttonNow == NOBOTON && flagButton != NOBOTON){
    flagButton = buttonNow;
  }
  delay(50);
}

// READ BUTTONS
/**
 * @brief  Reads the button that is pressed in the tension divisor.
 * @note   
 * @retval Returns the read button.
 */
int readButton(){
  int analogValue = analogRead(A0);
  if (analogValue >= 0 && analogValue <= 30){
      return GEARBOX_UP;
  }
  if (analogValue <= 371 && analogValue >= 311){
      return GEARBOX_DOWN;
  }
  if (analogValue <= 455 && analogValue >= 420){
      return RESTART;
  }
  return NOBOTON;
}

// LEDS
/**
 * @brief  It makes the red led blink if the maximum 
 * gearbox speed is between 85% and 105%.
 * @note   
 */
void blinkLed(){
  int stateControl = digitalRead(LED_R);
  int millisActual = millis();
  Serial.print("Speed: ");
  Serial.print(actualSpeed);
  Serial.println("KM/H");
  
  if(millisActual-millisBefore >= 500 && 
    actualSpeed >= maximunSpeed[gear]*0.85 &&
    actualSpeed <= maximunSpeed[gear]*1.05){
    flagGear = 1;
    digitalWrite(LED_R, !stateControl);
    stateControl = !stateControl;
    millisBefore = millisActual;
  }
}

/**
 * @brief  According to the percentage received, 
 * a green led turns on if it is greater than 0%, 
 * but a yellow one if it is greater than 75% or 
 * a red one will blink if it is greater than 90%.
 * @note   
 * @param  actualSpeed: Actual speed of the vehicle.
 * @param  maxSpeed: Maximun speed of the actual gear box.
 */
void turnOnLeds(int actualSpeed, int maxSpeed){
  float greenLedPercentaje = (maxSpeed*0.75);
  if(actualSpeed > 0){
      digitalWrite(LED_G, HIGH);
  }
  if(actualSpeed > greenLedPercentaje){
      digitalWrite(LED_Y, HIGH);
  }
  blinkLed();
}

/**
 * @brief  Shutdown all the leds of the board.
 * @note   
 */
void shutdownLeds(){
  for(int i = 10; i < 13; i++){
    digitalWrite(i, LOW);
  }
}

// GEARS
/**
 * @brief  Increase or decrease the current gear and 
 * show the new gear on the display.
 * @note   
 * @param  button: Button pressed to perform the action 
 * of raising or lowering the gear.
 */
void gearBoxControl(int button){
  if(button == GEARBOX_UP && !gear){
    gear++;
    flagGear = 0;
    shutdownLeds();
    accelerate(gear);
  }else if(button == GEARBOX_UP && gear <6 && flagGear){
    gear++;
    flagGear = 0;
    shutdownLeds();
    accelerate(gear);
  }
  if(gear>0 && button == GEARBOX_DOWN){
  	gear--;
    flagGear = 0;
    shutdownLeds();
    accelerate(gear);
  }
  showGearBox(gear);
}

// SPEED
/**
 * @brief  Regarding the actual gear, it will increase 
 * or decrease the actual speed.
 * @note   
 * @param  actualGear: Actual Gear to operate. 
 */
void accelerate(int actualGear){
  if(actualSpeed < maximunSpeed[actualGear]){
    actualSpeed+=acceleration;
    moveNeedle(actualSpeed);
    turnOnLeds(actualSpeed, maximunSpeed[actualGear]);
    delay(50);
  }
  else if(actualSpeed > maximunSpeed[actualGear]){
    actualSpeed-=acceleration;
    moveNeedle(actualSpeed);
    turnOnLeds(actualSpeed, maximunSpeed[actualGear]);
    delay(50);
  }
}

/**
 * @brief  Map the needle (servomotor) to move with 
 * the current speed within its limits.
 * @note   
 * @param  actualSpeed: Speed which will be mapped in degrees 
 * (between 0 and 180) for the movement of the needle (servomotor).
 */
void moveNeedle(int actualSpeed){
  int mapped = map(actualSpeed, 0, 180, 180, 0);
  needle.write(mapped);
}

// DISPLAY
/**
 * @brief  According to the change received by parameter, 
 * it will turn on the corresponding Display LEDs.
 * @note   
 * @param  gear: Actual Car's gear to show in the display.
 */
void showGearBoxOnDisplay(int gear){
  for(int i = 2; i < 9; i++){
    digitalWrite(i, carGearBox[gear][i-2]);
  }
}

/**
 * @brief  Prints the numeric value that represents the current change in the display.
 * @note   The possible values will be between 0 and 6.
 * @param  gear: Numeric Gear that will be represented in the display.
 */
void showGearBox(int gear){
  shutdownDisplay();
  showGearBoxOnDisplay(gear);
}

/**
 * @brief  Turn off all segments of the display.
 * @note   
 */
void shutdownDisplay(){
  for(int i = 2; i < 9; i++){
    digitalWrite(i, LOW);
  }
}

// RESTART
/**
 * @brief  Turns the program into its initial state.
 * @note   The display will show 0, the needle (Servo Motor) 
 * it will move to its initial position and the actual speed will be 0.
 */
void restartProgram(){
  gear = 0;
  actualSpeed = 0;
  showGearBox(0);
  moveNeedle(0);
  shutdownLeds();
}