/*************************************************************
 *
 * $Workfile: master.ino $
 *
 * $Creator: Jonathan Vargas $
 *
 * $Description:  $
 *************************************************************/

//! Include libraries
#include "Arduino.h"

//! Pinout definition
const int RELAY = 11;    //Relay
const int SLAVES[4] = {2, 3, 4, 5};   //Slaves signal
volatile int slavesNumber = 4; // Number of slaves attached 

//! Variables
boolean memory = false;

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {

  Serial.begin(9600);
  for (int i = 0; i < slavesNumber; i++)
  {
    pinMode(SLAVES[i], INPUT_PULLUP);
  }
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  delay(3000);
  
}

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void loop() {
  if(slavesState()) {
    digitalWrite(RELAY, HIGH);
  }
  else
  {
    digitalWrite(RELAY, LOW);
  }
  delay(500);
}

/***************************************************************************
 *
 * slavesState Function
 *
 * Returns true if all have find their RFID
 ***************************************************************************/
//! Reads state of all slaves attached. 
boolean slavesState(){
  boolean state = true;
  for (int i = 0; i < slavesNumber; i++)
  {
    uint8_t read = digitalRead(SLAVES[i]);
    //Serial.print(SLAVES[i]);
    //Serial.print(" --> ");
    //Serial.println(read);
    if(read==HIGH) state = false;  // If all slaves have read its RFID card, state is true

  }
  return state;
}
