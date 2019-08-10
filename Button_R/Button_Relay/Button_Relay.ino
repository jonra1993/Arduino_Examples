
/*************************************************************
 *
 * $Workfile: Button_Relay.ino $
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
#define wrong_led 10
#define correct_led 11

//! times pressed (pressing time is 400-600 millis)
unsigned long upper_press_interval=600;
unsigned long lower_press_interval=100;
//! times between pressed (break between pressing 400-600 millis)
unsigned long upper_break_interval=600;
unsigned long lower_break_interval=100;
//! times between secuence (break between 2-1-3-4 800-1200 millis)
unsigned long upper_wait_interval=1800;
unsigned long lower_wait_interval=800;
//! reset time intervals
unsigned long reset_interval_by_press=2000;
unsigned long reset_interval_by_time=8000;

// count count_presses
int state0_to1 = 2;
int state1_to2 = 1;
int state2_to3 = 3;
int state3_to4 = 4;


//! variables
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
volatile bool reset_memory = false;

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {
  pinMode(push_button,INPUT_PULLUP);
  pinMode(relay,OUTPUT);
  pinMode(wrong_led,OUTPUT);
  pinMode(correct_led,OUTPUT);

  digitalWrite(relay, HIGH);
  digitalWrite(wrong_led, LOW);
  digitalWrite(correct_led, LOW);

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
      if(conta_presses == state0_to1) {
        state = 1;
        state_change = true;
      }
      break;

    case 1:
      if(conta_presses == state1_to2 + state0_to1) {
        state = 2;
        state_change = true;
      }
      break; 

    case 2:
      if(conta_presses == state2_to3 + state1_to2 + state0_to1) {
        state = 3;
        state_change = true;
      }
      break;

    case 3:
      if(conta_presses == state3_to4 + state2_to3 + state1_to2 + state0_to1) {
        state = 4;
        state_change = true;
      }
      break; 
    
    case 4:
      digitalWrite(relay, LOW);
      digitalWrite(correct_led, HIGH);
      Serial.println("Correct sequence :)");
      Serial.println(" Press button for more than 2000 ms to reset system ");

      state = 6;
      state_change = false;
      break;

    case 5:
      if(conta_presses == 10) {
        digitalWrite(wrong_led, HIGH);
        Serial.println(" Incorrect sequence :( ");
        Serial.println(" Press button for more than 2000 ms to reset system ");
        state = 7;
        first_press = true;
        state_change = false;
      }

      break;

    case 6:
      break;

    case 7:
      break;    
  }

 
  //! Last time without press button
  if(digitalRead(push_button)==HIGH) {
    last_push_time_reset = millis();
  }

  // Reset detection
  if(digitalRead(push_button)==LOW) {
    if((millis() - last_push_time_reset) > reset_interval_by_press){
      reset();
      reset_memory = false;
    }
  }

  //reset by time elapse
  if (reset_memory == true && (conta_presses != 0) && (state != 6)){
    if((millis() - last_release_time_copy) > reset_interval_by_time){
      reset();
      reset_memory = false;
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
    reset_memory = true;
    if ((conta_presses == state3_to4 + state2_to3 + state1_to2 + state0_to1-1)){
      conta_presses++;
      attachInterrupt(digitalPinToInterrupt(push_button), release, FALLING);
    }
    else {
      attachInterrupt(digitalPinToInterrupt(push_button), release, RISING);
    }
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

    if(state == 5){
      conta_presses++;
    }
    else {
      conta_presses++;
      if (first_press == true){
        if(is_valid_pressed_time()) {
          first_press = false;
        }
      }
      else if (state_change == true){
        if(is_valid_pressed_time()) {
          if(is_valid_wait_time()){  
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
        if(is_valid_pressed_time()) {
          if(is_valid_release_time()){
          }
          else {
            state = 5;
          } 
        }
        else {
          state = 5;
        } 
      }
    }
    debugPrint();
    Serial.print("   Pressed time: ");
    Serial.print(pressed_time);
    Serial.print("  Released time: ");
    Serial.println(relesed_time);
    attachInterrupt(digitalPinToInterrupt(push_button), press, FALLING);
    last_release_time_copy = millis();
  }
}

/***************************************************************************
 *
 * reset Function
 *
 ***************************************************************************/

void reset() {
  conta_presses = 0;
  state = 0;
  first_press = true;
  state_change = false;
  attachInterrupt(digitalPinToInterrupt(push_button), press, FALLING);
  digitalWrite(correct_led, LOW);
  digitalWrite(wrong_led, LOW);
  Serial.println("System reseted!");
}

/***************************************************************************
 *
 * is_valid_pressed_time Function
 *
 ***************************************************************************/

bool is_valid_pressed_time() {
  return ((pressed_time >= lower_press_interval) && (pressed_time <= upper_press_interval));
}

/***************************************************************************
 *
 * is_valid_release_time Function
 *
 ***************************************************************************/

bool is_valid_release_time() {
  return ((relesed_time >= lower_break_interval) && (relesed_time <= upper_break_interval));
}

/***************************************************************************
 *
 * is_valid_wait_time Function
 *
 ***************************************************************************/

bool is_valid_wait_time() {
  return ((relesed_time >= lower_wait_interval) && (relesed_time <= upper_wait_interval));
}

/***************************************************************************
 *
 * debugPrint Function
 *
 ***************************************************************************/

void debugPrint() {
  Serial.print("count: ");
  Serial.print(conta_presses);
  Serial.print("  state: ");
  Serial.print(state);
}
