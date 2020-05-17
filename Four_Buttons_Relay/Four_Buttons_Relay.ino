
/************************************************************************************
 *
 * File Name:   Four_Button_Relay.ino 
 * Developer: Jonathan Vargas 
 * Web: www.jonathanvargas.ml
 * Date: 16/05/2020
 * Description:  4 buttons, sequence and relay
 * 
 ***********************************************************************************/

/******************************************************************************
	Connections (It is recommended to locate a ceramic capacitor between each button and GND to decrease noise)
  Relay   → pin 6
  Button1 → pin 7
  Button2 → pin 8
  Button3 → pin 9
  Button4 → pin 10
 *******************************************************************************/

//! Libraries included
#include "Arduino.h"

//! Push buttons definition
const int buttons_number = 4;
const int buttons[buttons_number] = {7,8,9,10};

//! Relay definition
#define RELAY 6

//! Debug Definitions
#define DEBUG true              // true for printing DEBUG info
     
//! Variables
volatile byte state = 0;
int buttonState[buttons_number];     // current state of the button
int lastButtonState[buttons_number]; // previous state of the button
long startPressed[buttons_number];    // the time button was pressed
long endPressed[buttons_number];      // the time button was released
long timeHold[buttons_number];        // the time button was hold
long timeReleased[buttons_number];    // the time button was released
bool music_on = false;

//! secuence
volatile uint8_t seq[buttons_number] = {1, 2, 3, 4};  //This is the button sequence that should be pushed

int valid_time_interval[buttons_number] = {1, 1, 1, 1};
volatile uint8_t seq_tempo[buttons_number] = {0, 0, 0, 0};
volatile uint8_t counter = 0;
volatile uint8_t last_counter = 0;
volatile uint8_t wrong_tries = 0;
bool valid_time = false;
bool correct = false;

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {
	#if DEBUG == true
		// Initialize hardware serial port (serial debug port)
		Serial.begin(9600);
		// Wait for serial port to connect. Needed for native USB port only
		while (!Serial);
		Serial.println("");
		Serial.println("==============================");
		Serial.println("==== Four_Button_Relay Debug ===");
		Serial.println("==============================");
	#endif
	// Relays Initialization
	pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

	// Buttons Initialization
	for (int i = 0; i < buttons_number; i++) {
		pinMode(buttons[i],INPUT_PULLUP);
  }

	// Init arrays
  for (int i = 0; i < buttons_number; i++) {
		buttonState[i] = 0;    
	  lastButtonState[i] = HIGH; 
		startPressed[i] = 0;  
		endPressed[i] = 0; 
		timeHold[i] = 0;  
		timeReleased[i] = 0;  
  }
}

/***************************************************************************
 *
 * Loop Function
 *
 ***************************************************************************/
void loop() {
  //! state machine
  switch(state)
  {
    case 0:
    	// Read push buttons
      for (int i = 0; i < buttons_number; i++) {
        buttonState[i] = digitalRead(buttons[i]); // read the button input
        if (buttonState[i] != lastButtonState[i]) { // button state changed
          updateState(i+1, buttonState[i], startPressed[i], endPressed[i], timeHold[i], timeReleased[i], valid_time_interval[i]);
        }
        lastButtonState[i] = buttonState[i]; 
      }
      if(last_counter!=counter){
        state = 1;
        last_counter = counter;
      }
      break;

    case 1:
      state = 2;
      correct = true;
			for(int i = 0; i<counter; i++){
				Serial.print(seq_tempo[i]);
				Serial.print(" ,");
				if(seq[i]!=seq_tempo[i]){
					correct = false;
          Serial.println("INCORRECT SEQUENCE TRY AGAIN !!");
					return;
				}
			}
      break; 

    case 2:
      if(correct){
        if(counter >= buttons_number){
          Serial.println("CORRECT SEQUENCE !!");
          digitalWrite(RELAY, LOW);
				  state = 3;
        }
        else{
           state = 0;
        }
        
			}
			else{
				state = 0;
        counter = 0;
        last_counter = 0;
			}
      break;

    case 3:
      break;

    default:
      break; 
  }

}

/***************************************************************************
 *
 * updateState Function 
 * 
 * Description: 
 *
 ***************************************************************************/
void updateState( int button, 
                  int &buttonState, 
                  long &startPressed, 
                  long &endPressed, 
                  long &timeHold, 
                  long &timeReleased,
                  int &validTime) {
  // the button was just pressed
  if (buttonState == LOW) {
    startPressed = millis();
    timeReleased = startPressed - endPressed;

    // #if DEBUG == true
    //   Serial.print("Button");
    //   Serial.print(button);
    //   Serial.print(" was idle for ");
    //   Serial.print(timeReleased);
    //   Serial.println(" [ms]");
    // #endif
    // the button was just released
  } 
  else {
    endPressed = millis();
    timeHold = endPressed - startPressed;

    bool already_pressed = false;
    for(int i = 0; i<counter; i++){
      if(seq_tempo[i] == button){
        already_pressed = true;
      }
    }
    if(already_pressed ==false){
      seq_tempo[counter] = button;
      counter++;
    }    

    if(timeHold>validTime*1000) valid_time = true;
    else valid_time = false;

    #if DEBUG == true
      Serial.print("Button");
      Serial.print(button);
      Serial.print(" was hold for ");
      Serial.print(timeHold);
      Serial.println(" [ms]");
    #endif
  }
}
