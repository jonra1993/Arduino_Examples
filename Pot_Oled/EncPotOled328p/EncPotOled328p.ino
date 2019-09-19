
/*************************************************************
 *
 * $Workfile: EncPotOledNano.ino $
 *
 * $Creator:  $
 *
 * $Description:  $
 *************************************************************/

//! Include libraries
#include <EEPROM.h>
#include "U8glib.h"
U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI

//! sensorValue eeprom address
int encCounterAddress = 0;

//! Pinout definitions
#define POT A0
#define ENC_A 2
#define ENC_B 4

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
void setup()
{
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);

  if (conType)
  {
    attachInterrupt(0, int0, CHANGE);
    //EEPROM.put(encCounterAddress, 20);
    EEPROM.get( encCounterAddress, encCounterLast );
    encCounter = encCounterLast;
  }
  delay(500);
}
/***************************************************************************
 *
 * loop Function
 *
 ***************************************************************************/
void loop()
{
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
  char sensorValue_str[6];
  dtostrf(sensorValue, 5, 1, sensorValue_str);
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_fub20r);
    u8g.setPrintPos(10, 25);
    u8g.print(sensorValue_str);
  } while ( u8g.nextPage() );

  delay(10);
}

/***************************************************************************
 *
 * int0 Function
 *
 ***************************************************************************/
//! Detects when a change is generated in D2
void int0()
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
 * makeBackup Function
 *
 ***************************************************************************/
//! Detects a FALLING edge is generated in D3 and saves ecCounter value in eeprom
void makeBackup()
{
  encCounterLast = encCounter;
  EEPROM.put(encCounterAddress, encCounterLast);
}
