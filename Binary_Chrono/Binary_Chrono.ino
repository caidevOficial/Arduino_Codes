/*
 * MIT License
 * 
 * Copyright (c) 2021 [Lucas Ferrini - FacuFalcone]
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
 *	Mini Proyecto 1: Cronómetro Binario
 *
 *	Al iniciar la simulacion, todos los led deben comenzar apagados.
 *	Cada 1 segundo que transcurra, se deben encender los led
 *	de acuerdo al numero que representan.
 *
 *	Ejemplo:
 *   
 *	El numero 127 (00001111111) se representaria encendiendo los 
 *	leds AMARILLO, NARANJA y ROJO
 *	Al pasar al numero 128 (00010000000), se deben apagar todos 
 *	los led anteriores y encenderse uno de los leds VERDES. 
 *	Notese, en el ejemplo los 0 representan los led apagados 
 *	y los 1 los led encendidos). 
 *   
 *	-------------------------------------------------------
 *   
 *	Al presionarse el boton, debe detenerse el cronometro.
 *	Volver a presionarlo hace que la secuencia continue.
 *	De mantenerse el boton pulsado por 3 segundos, el cronometro
 *	debe reiniciarse.
 *
 *	Tip: Modularizar la función que controla el encendido de los 
 *	LEDS y de ser posible, todo el código. Usar millis para 
 *	controlar el tiempo del contador para que el cambio de los 
 *	leds encendidos sea perceptible para el ojo humano y 
 *	documentar cada función creada en el código. 
 *	(Un breve comentario que diga que es lo que hace esa función).
 *   
*/

//--- Defines ---//
#define ZERO_TO_HERO 0
#define SIZE 12
#define BUTTON_PIN 2
#define FIRST_LED 3
#define LAST_LED 13
#define SECONDS 1000
//--- End Defines ---//

//--- Variables ---//
int secondsCounter = ZERO_TO_HERO;
int buttonBeforeState = LOW;
int buttonNowState;
int initialized = 0;
int binaryString[SIZE];
int mainMillisBefore = 0;
unsigned long mainCurrentMillis;
//--- End Variables ---//

//--- Setup ---//
void setup() {
    Serial.begin(9600);
    // Inicializar el cronometro
    for (int i = 3; i < 14; i++) {
        pinMode(i, OUTPUT);
    }
    pinMode(BUTTON_PIN, INPUT);
}
//--- End Setup ---//

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
    unsigned long currentMillis = millis();
    int millisBefore = 0;
    if(currentMillis > millisBefore+secondsUnitTime){
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
    // Controlar el boton
    buttonNowState = digitalRead(BUTTON_PIN);
    if(buttonNowState == HIGH && buttonBeforeState == LOW) {
      initialized = !initialized;
    }
    buttonBeforeState = buttonNowState;
    
    if(!initialized){
        // If the button is pressed, the counter stops
        secondsCounter = ZERO_TO_HERO;
        shutDownLeds();
    }else{
        // If the button is pressed, the counter starts
        if(millis() > mainMillisBefore+SECONDS){
            mainMillisBefore = millis();
            secondsCounter = calculateSeconds(secondsCounter, SECONDS);
            decimalToBinary(secondsCounter , binaryString);
            ShowBinaryString(binaryString);
            printMessage(secondsCounter, binaryString);
        }
    }

    delay(25);
}
