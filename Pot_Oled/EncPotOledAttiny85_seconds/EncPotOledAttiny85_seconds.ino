/*************************************************************
 *
 * $Workfile: EncPotOledAttiny85.ino $
 *
 * $Creator:  $
 *
 * $Description:  $
 *************************************************************/

//! Include libraries
#include <EEPROM.h>
#include <Tiny4kOLED.h>
#include "font11x16.h"

//! sensorValue eeprom address
int encCounterAddress = 0;

//! Pinout definitions
#define POT 3
#define ENC_A 4      
#define ENC_B 1

#define ENC_TYPE 0

//! Variables
float sensorValue = 0;
volatile int encCounter;
int encCounterLast;
volatile boolean state0, lastState, turnFlag;

/*------------------Select interval------------------------*/
const int interval = 160;  //0....160
//const int interval = 300;    //0....300
/*------------------Select interval------------------------*/

/*------------------Control type Select------------------------*/
bool conType = true; //Encoder
//bool conType = false; //Potentiometer
/*------------------Control type Select------------------------*/

/*------------------Allows eeprom backup------------------------*/
bool backup = true; 
/*------------------Allows eeprom backup------------------------*/

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {

  initOled();
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);

  if (conType)
  {
    //encCounter = interval;
    //EEPROM.put(encCounterAddress, 20);
    EEPROM.get( encCounterAddress, encCounterLast );
    encCounter = encCounterLast;

  }
  delay(500);
  pcintSetup();
}

/***************************************************************************
 *
 * loop Function
 *
 ***************************************************************************/
void loop() {
  if (conType)
  {
    sensorValue = encCounter/2;
    if (sensorValue < 0.0) sensorValue = 0.0;
    if (sensorValue > interval) sensorValue = interval;
  }
  else
  {
    sensorValue = analogRead(POT);
    int tmpInt = interval * 10;
    sensorValue = map(sensorValue, 0, 1023, 0, tmpInt);
    sensorValue = sensorValue / 10.0;
  }
  updateDisplay(sensorValue);
  delay(50);
}

/***************************************************************************
 *
 * updateDisplay function
 *
 ***************************************************************************/
//! Update new values at oled
void updateDisplay(float number) {
  // Clear the half of memory not currently being displayed.
  oled.clear();
  // Position the text cursor
  // In order to keep the library size small, text can only be positioned
  // with the top of the font aligned with one of the four 8 bit high RAM pages.
  // The Y value therefore can only have the value 0, 1, 2, or 3.
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(0, 0);
  // Write text to oled RAM (which is not currently being displayed).
  oled.print(F("  Seconds:"));
  // Write the number of milliseconds since power on.
  number = 14;
  oled.setCursor(16, 2);
  oled.print(number);
  // Swap which half of RAM is being written to, and which half is being displayed.
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
  oled.switchFrame();
}

/***************************************************************************
 *
 * PCINT interrupt setup
 *
 ***************************************************************************/
//! Makes register configuration trigger by PCINT4
void pcintSetup()
{
  GIMSK |= (1 << PCIE);   // pin change interrupt enable
  PCMSK |= (1 << PCINT4); // pin change interrupt enabled for PCINT4
  sei();                  // enable interrupts
}

/***************************************************************************
 *
 * ISR vector interrupt
 *
 ***************************************************************************/
//! PCINT0 interrupt trigger
ISR (PCINT0_vect) 
{     
  state0 = digitalRead(ENC_A);
  if (state0 != lastState)
  {
#if (ENC_TYPE == 1)
    turnFlag = !turnFlag;
    if (turnFlag)
      encCounter += (digitalRead(ENC_B) != lastState) ? -1 : 1;
#else
    encCounter += (digitalRead(ENC_B) != lastState) ? -1 : 1;
#endif
    lastState = state0;
    if(backup) makeBackup(); //backup data each time it has a new value
  }
}

/***************************************************************************
 *
 * initOled Function
 *
 ***************************************************************************/
//! inits SSD 1306 Oled 128x32
void initOled()
{
  //! Send the initialization sequence to the oled. This leaves the display turned off
  oled.begin();
  //oled.setFont(FONT11X16);
  oled.setFont(FONT8X16);
  //! Clear the memory before turning on the display
  oled.clear();
  //! Turn on the display
  oled.on();
  //! Switch the half of RAM that we are writing to, to be the half that is non currently displayed
  oled.switchRenderFrame();
}

/***************************************************************************
 *
 * makeBackup Function
 *
 ***************************************************************************/
//! saves ecCounter value in eeprom
void makeBackup()
{
  encCounterLast = encCounter;
  EEPROM.put(encCounterAddress, encCounterLast);
}
