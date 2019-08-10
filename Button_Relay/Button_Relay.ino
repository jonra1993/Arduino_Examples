
/*************************************************************
 *
 * $Workfile: Bluetooth_Relay.ino $
 *
 * $Creator: Jonathan Vargas $
 *
 * $Description:  $
 *************************************************************/

//! Libraries included
#include "Arduino.h"

//! definitions
#define push_button 2
#define relay 9
#define no_relay 10

//! times pressed (pressing time is 400-600 millis)
unsigned long upper_press_interval=600;
unsigned long lower_press_interval=100;
//! times pressed (break between pressing 400-600 millis)
unsigned long upper_break_interval=600;
unsigned long lower_break_interval=100;
//! times pressed (break between 2-1-3-4 800-1200 millis)
unsigned long upper_wait_interval=1800;
unsigned long lower_wait_interval=800;

//! variables
unsigned long reset_interval=2000;
volatile int conta_presses = 0;
volatile byte state = 0;
volatile unsigned long last_push_time;
volatile unsigned long last_push_time_reset;
volatile unsigned long last_release_time;
volatile unsigned long last_release_time_copy;
volatile unsigned long pressed_time;
volatile unsigned long relesed_time;
volatile unsigned long current_time;
volatile bool valid_press = false;
volatile bool valid_break = false;
volatile bool first_press = true;
volatile bool state_change = false;

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {
  pinMode(push_button,INPUT_PULLUP);
  pinMode(relay,OUTPUT);
  pinMode(no_relay,OUTPUT);

  //! Configure pin interrupt
  attachInterrupt(digitalPinToInterrupt(push_button), press, FALLING); //FALLING / RISING
  Serial.begin(19200);
  delay(500);

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
      if(conta_presses == 2) {
        conta_presses = 0;
        state = 1;
        state_change = true;
      }
      break;

    case 1:
      if(conta_presses == 1) {
        conta_presses = 0;
        state = 2;
        state_change = true;
      }
      break; 

    case 2:
      if(conta_presses == 3) {
        conta_presses = 0;
        state = 3;
        state_change = true;
      }
      break;

    case 3:
      if(conta_presses == 4) {
        state = 4;
        state_change = true;
      }
      break; 
    
    case 4:
      digitalWrite(relay, HIGH);
      digitalWrite(no_relay, LOW);
      Serial.println("Correct secuence :)");
      state = 6;
      state_change = false;
      break;

    case 5:
      digitalWrite(relay, LOW);
      digitalWrite(no_relay, HIGH);
      Serial.println(" Incorrect secuence :( ");
      Serial.println(" Press button for more than 2000 ms to reset system ");
      state = 6;
      first_press = true;
      state_change = false;
      break;

    case 6:
      break;    
  }

 
  //! Last time without press button
  if(digitalRead(push_button)==HIGH) {
    last_push_time_reset = millis();
  }

  // Reset detection
  if(digitalRead(push_button)==LOW) {
    if((millis() - last_push_time_reset) > reset_interval){
      conta_presses = 0;
      state = 0;
      first_press = true;
      state_change = false;
      attachInterrupt(digitalPinToInterrupt(push_button), press, FALLING);
      digitalWrite(relay, LOW);
      digitalWrite(no_relay, LOW);
      Serial.println("System reseted!");
    }
  }
}

/***************************************************************************
 *
 * press Function
 *
 ***************************************************************************/
//! Detect when button is pressed
void press() {
  //! Debounce software filter
  delay(20);
  if(digitalRead(push_button)==LOW) {
    last_push_time = millis(); 
    relesed_time = last_push_time-last_release_time_copy; 
    attachInterrupt(digitalPinToInterrupt(push_button), release, RISING);
  }
}

/***************************************************************************
 *
 * release Function
 *
 ***************************************************************************/
//! Detect when button is release
void release() {
  //! Debounce software filter
  delay(20);
  if(digitalRead(push_button)==HIGH) {
    last_release_time = millis();
    pressed_time = last_release_time-last_push_time;
    Serial.print("Pressed time: ");
    Serial.print(pressed_time);
    Serial.print("  Released time: ");
    Serial.println(relesed_time);

    if (first_press == true){
      if((pressed_time>lower_press_interval) && (pressed_time < upper_press_interval)) {
        conta_presses++;
        debugPrint(conta_presses, state);
        first_press = false;
      }
    }

    else if (state_change == true){
      if((pressed_time > lower_press_interval) && (pressed_time < upper_press_interval)) {
        if((relesed_time > lower_wait_interval) && (relesed_time < upper_wait_interval)){
          conta_presses++;
          debugPrint(conta_presses, state);
          state_change = false;
        }
        else {
          state = 5;
        }
      }
      else {
        state = 5;
      } 
    }

    else {
      if((pressed_time > lower_press_interval) && (pressed_time < upper_press_interval)) {
        if((relesed_time > lower_break_interval) && (relesed_time < upper_break_interval)){
          conta_presses++;
          debugPrint(conta_presses, state);
        }
        else {
          state = 5;
        } 
      }
      else {
        state = 5;
      } 
    }
    attachInterrupt(digitalPinToInterrupt(push_button), press, FALLING);
    last_release_time_copy = millis();
  }
}


/***************************************************************************
 *
 * debugPrint Function
 *
 ***************************************************************************/

void debugPrint(volatile unsigned long conta, volatile bool state) {
  Serial.print("count: ");
  Serial.print(conta);
  Serial.print("  state: ");
  Serial.println(state);
}
