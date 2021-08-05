/*
 * 1 - Al inicar el programa, el velocímetro debe estar en 0 con el cambio 0 
 *    y una velocidad maxima de 0.
 * 2 - Al subir un cambio, pulsando su boton correspondiente, la velocidad maxima incrementa en 30, 
 *    con lo que la velocidad empieza a aumentar por la aceleracion.
 * 3 - Al disminuirlo, pulsando su boton correspondiente, la velocidad maxima decrementa en 30. 
 *    Mientras la velocidad máxima sea menor que la velocidad actual, la velocidad debe disminuir.
 * 4 - Respecto a la velocidad actual:
 *  A - Si la velocidad actual supera el 70% de la velocidad máxima, se debe encender el led verde. 
 *  B - Si supera el 80% debe encenderse el amarillo. 
 *  C - Si supera el 90% debe encenderse el rojo. 
 *  NOTA: Los cambios solo pueden pasarse cuando el led rojo esta encendido, caso contrario no debera dejarte
 *        por mas que se presione el boton para subir cambios.
 *  5 - El servo debe iniciar completamente del lado izquierdo y moverse gradualmente a la derecha a medida 
 *    que la velocidad aumenta o viceversa.
*/
#include <Servo.h>

Servo needle;
#define SERVO 9

//BOTONES
#define NOBOTON 0
#define GEARBOX_UP 1
#define GEARBOX_DOWN 2
#define RESTART 3

// LEDS
#define LED_R 10
#define LED_Y 11
#define LED_G 12

int millisBefore = 0; // para controlar el parpadeo del LED
int flagButton = NOBOTON; // Antirebote
int gear = 0;
int flagGear = 0; // Para saber si se puede o no pasar un cambio
int actualSpeed = 0;
int maximunSpeed[7] = {0, 30, 60, 90, 120, 150, 180};
int acceleration = 1; // Este valor se suma o resta a la velocidad actual
// lEDS DEL DISPLAY REFERIDO A LOS CAMBIOS DEL AUTO
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

// LECTURA BOTONES
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

// CAMBIOS
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

// VELOCIDAD
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