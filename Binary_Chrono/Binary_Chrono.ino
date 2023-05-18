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

/*
 *	Trabajo Práctico 1: Cronómetro Binario
 *
 *	Al iniciar la simulacion, todos los led deben comenzar apagados.
 *	Cada 1 segundo que transcurra, se deben encender los led
 *	de acuerdo al numero que representan.
 *
 *	Ejemplo:
 *   
 *	El numero 15 (0000001111) se representaria encendiendo los 
 *	leds AMARILLO y NARANJA.
 *	Al pasar al numero 16 (0000010000), se deben apagar todos 
 *	los led anteriores y encenderse uno de los leds VERDES. 
 *	Notese, en el ejemplo los 0 representan los led apagados 
 *	y los 1 los led encendidos). 
 *   
 *	-------------------------------------------------------
 *   
 *	Al presionarse el boton START, debe iniciar el cronometro.
 *	Volver a presionarlo hace que la secuencia se detenga. 
 *  (Como si se hubiese presionado pausa).
 *	Al presionarse el boton RESET, el cronometro
 *	debe reiniciarse y arrancar de 0.
 *
 *	Tip: Modularizar la función que controla el encendido de los 
 *	LEDS y de ser posible, todo el código para evitar repetir lineas lo mas posible.
 *  Usar millis para controlar el tiempo del contador para que el cambio de los 
 *	leds encendidos sea perceptible para el ojo humano y 
 *	documentar cada función creada en el código. 
 *	(Un breve comentario que diga que es lo que hace esa función
 *  y de corresponder, que retorna).
 *   
*/

//--- Defines ---//
#define ZERO_TO_HERO 0 //! Comment this line to use the below version
//#define ZERO_TO_HERO 1015 //! To check a value near the max limit of the leds.
#define SIZE 11
#define BUTTON_START 2
#define BUTTON_RESET 3
#define FIRST_LED 4
#define LAST_LED 13
#define BASE_MILLI_SECONDS 1000
#define MAX_SECONDS 1023
//--- End Defines ---//

//--- Variables ---//
unsigned long long secondsCounter = ZERO_TO_HERO;
int buttonBeforeStart = LOW;
int buttonNowStart;
int buttonBeforeReset = LOW;
int buttonNowReset;
int reset = 0;
int paused = 1;
int binaryString[SIZE];
unsigned long long mainMillisBefore = 0;
unsigned long long mainCurrentMillis;
//--- End Variables ---//

//--- Setup ---//
void setup() {
    Serial.begin(9600);
    // Inicializar el cronometro
  	setPins(FIRST_LED, LAST_LED, OUTPUT);
    setPins(2, 3, INPUT);
}
//--- End Setup ---//

/**
 * @brief Function that setup the mode of the pins.
 * @note  This function is used to setup the mode of the pins
 *        from 'minPin' to 'maxPin' in the mode 'mode'[INPUT or OUTPUT].
 */
void setPins(int minPin, int maxPin, int mode){
	for (int i = minPin; i <= maxPin; i++) {
        pinMode(i, mode);
    }
}

//--- Leds Handling ---//
/**
 * @brief Function that shuts down the leds.
 * @note  This function is used to shut down the leds.
 */
void shutDownLeds() {
    for (int i = 3; i < 14; i++) {
        digitalWrite(i, LOW);
    }
}
    
/**
 * @brief Function that turns on the leds according to the binary string.
 * @note This function will set the state of the leds according to the binary string. 
 *          1 to turn on the led, 0 to turn off the led.
 * @param  binaryString: The binary string that will be used to turn on the leds.
 */
void ShowBinaryString(int* binaryString) {
    int position = SIZE-1;
    for (int i = FIRST_LED; i <= LAST_LED ; i++) {
        digitalWrite(i, binaryString[position]);
        position--;
    }
}
//--- End Leds Handling ---//

//--- Calculus & Convertions ---//
/**
 * @brief Function that calculates the seconds.
 * @param  secondsUnitTime: The time of the seconds.
 * @retval Every second calculated.
 */
int calculateSeconds(int secondsCounter, int secondsUnitTime) {
    unsigned long long currentMillis = millis();
    unsigned long long  millisBefore = 0;
    unsigned long long timeSum = millisBefore+secondsUnitTime;
    if(currentMillis > timeSum){
      	millisBefore=currentMillis;
  		secondsCounter++;
    }
    return secondsCounter;
}

/**
 * @brief Function that calculates the binary number of the seconds.
 * @note This function will save the binary number of the seconds in the array.
 * @param  decimalNumber: The number of the seconds to be converted.
 * @param  binaryString: The array where the binary number will be saved.
 */
void decimalToBinary(int decimalNumber, int* binaryString) {
    int binaryNumber = 0;
    int lastIndex = SIZE-1;
    
    while (decimalNumber > 0) {
        binaryNumber = decimalNumber % 2;
        decimalNumber /= 2;
        binaryString[lastIndex] = binaryNumber;
        lastIndex--;
    }

    while (lastIndex >= 0) {
        binaryString[lastIndex] = 0;
        lastIndex--;
    }
}
//--- End Calculus & Convertions ---//

//--- Serial Port Messages ---//
/**
 * @brief Function that prints the binary number of the array.
 */
void printsBinaryNumber(){
    for (int i = 0; i < SIZE; i++) {
        Serial.print(binaryString[i]);
    }
  Serial.println();
}

/**
 * @brief Function that prints the message with the seconds and the binary number.
 * @note The mesage will be like 'Secons: xxx | Binary: xxx'.
 * @param  seconds: The number of the seconds to be printed.
 * @param  binaryString: The array with the binary number to be printed.
 */
void printMessage(int seconds, int* binaryString) {
    Serial.print("Seconds: ");
    Serial.print(seconds);
    Serial.print(" | Binary: ");
    printsBinaryNumber();
}
//--- End Serial Port Messages ---//

void loop() {
    // Control the button.
    buttonNowStart = digitalRead(BUTTON_START);
    buttonNowReset = digitalRead(BUTTON_RESET);
    
    if(buttonNowStart == HIGH && buttonBeforeStart == LOW) {
      paused = !paused;
    }
    buttonBeforeStart = buttonNowStart;
    
    if(buttonNowReset == HIGH && buttonBeforeReset == LOW) {
      reset = !reset;
    }
    buttonBeforeReset = buttonNowReset;
    
    //? if isn't initialized or the secondsCounter is bigger than the max seconds
    //* then the secondsCounter will be reset to 0 and the leds will be shut down.
    if(reset || secondsCounter > MAX_SECONDS) {
        secondsCounter = ZERO_TO_HERO;
        shutDownLeds(); //? shutdown all the leds to proceed with the reset process.
        reset = !reset; //* Update the value of the reset variable.
    }else if(!paused && millis() > (mainMillisBefore+BASE_MILLI_SECONDS)){
        mainMillisBefore = millis();
        secondsCounter = calculateSeconds(secondsCounter, BASE_MILLI_SECONDS);
        //? if the secondsCounter are smaller than the max seconds then
        //* - the binary number will be calculated
        //* - the binary string will be shown with the leds
        //* - the message will be printed
        if(secondsCounter <= MAX_SECONDS) {
            decimalToBinary(secondsCounter , binaryString);
            ShowBinaryString(binaryString);
            printMessage(secondsCounter, binaryString);
        }
    }

    delay(7);
}
