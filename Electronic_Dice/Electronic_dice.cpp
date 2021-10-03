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

//--- Defines ---//
#define btnRandom 2
#define btnUp 4
#define btnDown 3
#define ROWS 7
#define COLUMNS 7
//--- End Defines ---//

//--- Variables ---//
int arduDice = 0;
int btnRandomBefore = LOW;
int btnUpBefore = LOW;
int btnDownBefore = LOW;
int diceMAtrix[ROWS][COLUMNS] = {/*Pins:
      5,    6,    7,    8,    9,   10,   11*/
    {LOW,  LOW,  LOW,  LOW,  LOW,  LOW,  LOW}, // Dice: OFF
    {LOW,  LOW,  LOW,  HIGH, LOW,  LOW,  LOW}, // Dice: 1
    {HIGH, LOW,  LOW,  LOW,  HIGH, LOW,  LOW}, // Dice: 2
    {HIGH, LOW,  LOW,  HIGH, HIGH, LOW,  LOW}, // Dice: 3
    {HIGH, LOW,  HIGH, LOW,  HIGH, LOW,  HIGH}, // Dice: 4
    {HIGH, LOW,  HIGH, HIGH, HIGH, LOW,  HIGH}, // Dice: 5
    {HIGH, HIGH, HIGH, LOW,  HIGH, HIGH, HIGH}  // Dice: 6
};
//--- End Variables ---//

//--- Setup ---//
/**
 * @brief This function is the main function of the program.
 * @note Used for configure the pins and the buttons.
 */
void setup() {
    Serial.begin(9600);
    setInputs();
    setOutputs();
}
//--- End Setup ---//

//--- Inputs & Outputs ---//
/**
 * @brief  Sets the inputs of the board
 * @note   The inputs are: 2 to 4
 */
void setInputs() {
    for (int i = 2; i < 5; i++) {
        pinMode(i, INPUT);
    }
}

/**
 * @brief Sets the outputs of the board
 * @note The outputs are: 5 to 11
 */
void setOutputs() {
    for (int i = 5; i < 12; i++) {
        pinMode(i, OUTPUT);
    }
}
//--- End Inputs & Outputs ---//

//--- Random Numbers ---//
/**
 * @brief  This will prints a random number between 1 and 6 in a specific
 *         period of time.
 * @param  period: The period of time in milliseconds to print every number.
 * @param  arduDice: The dice number to print.
 */
void printRandomUsingMillis(int period, int arduDice) {
    int randomTemporal;
    int contador=0;
    int millisBefore=0;
    unsigned long millisActual=millis();
    
    if((millisActual-millisBefore) >= period){
      	millisBefore=millisActual;
  		showNumber(arduDice);
    }
}

/**
 * @brief This will show a number of the dice randomly between 1 and 6.
 * @note  The interval will be managed by the program.
 * @param min The minimun number of the times that the dice could be shown.
 * @param max The maximun number of the times that the dice could be shown.
 */
void randomDice(int min, int max) {
    for(int i = 0; i < random(min,max); i++) {
            arduDice = random(1, 7); // 1.00000000001 --- 6.99999999999
            printRandomUsingMillis(300, arduDice);
            
            delay(500);
        }
}
//--- End Random Numbers ---//

//--- Buttons ---//
/**
 * @brief Reads the button 'random' and shows the dice number.
 * @note  The button 'random' is the pin 2.
 * @retval Returns the state of the button 'random' after the reading.
 */
int readBtnRandom(int btnRandomNow, int btnRandomBefore) {
    int min = random(4, 9);
    int max = random(9, 15);
    if (btnRandomNow == HIGH && btnRandomBefore == LOW) {
        randomDice(min, max);
    }

    return btnRandomNow;
}

/**
 * @brief Reads the button 'up' and increments the dice number.
 * @note The button 'up' is the pin 4.
 * @retval Returns the state of the button 'up' after the reading.
 */
int readBtnUp(int btnUpNow, int btnUpBefore) {
    if (btnUpNow == HIGH && btnUpBefore == LOW) {
        if (arduDice < 6) {
            arduDice++;
            showNumber(arduDice);
        }
    }

    return btnUpNow;
}

/**
 * @brief Reads the button 'down' and decrements the dice number.
 * @note The button 'down' is the pin 3.
 * @retval Returns the state of the button 'down' after the reading.
 */
int readBtnDown(int btnDownNow, int btnDownBefore) {
    if (btnDownNow == HIGH && btnDownBefore == LOW) {
        if (arduDice > 1) {
            arduDice--;
            showNumber(arduDice);
        }
    }

    return btnDownNow;
}
//--- End Buttons ---//

//--- Show Dice Number ---//
/**
 * @brief Shows the dice number on the LEDs.
 * @note The LEDs are the pins 5 to 11.
 * @param  diceValue: The dice number to be shown.
 */
void showNumber(int diceValue) {
    printMessage(arduDice);
    for(int i = 0; i < 8; i++) {
        digitalWrite(i+5, diceMAtrix[diceValue][i]);
    }
}
//--- End Show Dice Number ---//

//--- Messages ---//
/**
 * @brief Prints a message on the serial port with the dice number.
 * @param  number: The dice number to be shown.
 */
void printMessage(int number){
    Serial.println("------");
    Serial.print("Dice Number: ");
    Serial.println(number);
    Serial.println("------");
}
//--- End Messages ---//

//--- Main ---//
/**
 * @brief Main loop function of the program.
 * @note The loop function is called every time the program is executed.
 */
void loop(){
    int btnRandomNow = digitalRead(btnRandom);
    int btnUpNow = digitalRead(btnUp);
    int btnDownNow = digitalRead(btnDown);

    btnRandomBefore = readBtnRandom(btnRandomNow, btnRandomBefore);
    btnUpBefore = readBtnUp(btnUpNow, btnUpBefore);
    btnDownBefore = readBtnDown(btnDownNow, btnDownBefore);

    delay(25); // unique delay of the program.
}
//--- End Main ---//
