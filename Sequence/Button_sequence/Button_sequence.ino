
/*************************************************************
 *
 * $Workfile: Button_sequence.ino $
 *
 * $Creator: Jonathan Vargas $
 *
 * $Description:  $
 *************************************************************/

//! Libraries included
#include "Arduino.h"
#include "Pushbutton.h"

//! definitions
#define PUSH_BUTTON_1 2
#define PUSH_BUTTON_2 3
#define PUSH_BUTTON_3 4
#define PUSH_BUTTON_4 5

#define RELAY_MAG_LOCK_1 22
#define RELAY_MAG_LOCK_2 23
#define RELAY_MAG_LOCK_3 24
#define RELAY_MAG_LOCK_4 25
#define RELAY_MASTER 26
#define RELAY_SLAVE_1 27
#define RELAY_SLAVE_2 28
#define RELAY_SLAVE_3 29
#define RELAY_SLAVE_4 30

#define CORRECT_LED 7
#define INCORRECT_LED 8

//! variables
int time_for_checking = 30;  //Time in seconds after system will check  
int time_correct = 180;  //Time in seconds after correct
int time_correct_now = 0;  //Time in seconds after correct  
int time_elapsed = 0;
int period = 1000;
unsigned long time_now = 0;
uint8_t button_pressed = 0;
uint8_t state = 0;
//! secuence
volatile uint8_t seq[6] = {3,2,1,2,4,3};
volatile uint8_t seq_tempo[6] = {0,0,0,0,0,0};
volatile uint8_t counter = 0;
bool correct;

//! pushbutton objects
Pushbutton push_button_1(PUSH_BUTTON_1);
Pushbutton push_button_2(PUSH_BUTTON_2);
Pushbutton push_button_3(PUSH_BUTTON_3);
Pushbutton push_button_4(PUSH_BUTTON_4);

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {
	Serial.begin(9600);

  pinMode(RELAY_MAG_LOCK_1,OUTPUT);
  pinMode(RELAY_MAG_LOCK_2,OUTPUT);
  pinMode(RELAY_MAG_LOCK_3,OUTPUT);
  pinMode(RELAY_MAG_LOCK_4,OUTPUT);
  pinMode(RELAY_SLAVE_1,OUTPUT);
  pinMode(RELAY_SLAVE_2,OUTPUT);
  pinMode(RELAY_SLAVE_3,OUTPUT);
	pinMode(RELAY_SLAVE_4,OUTPUT);
	pinMode(RELAY_MASTER,OUTPUT);
	pinMode(CORRECT_LED,OUTPUT);
	pinMode(INCORRECT_LED,OUTPUT);

  digitalWrite(RELAY_MAG_LOCK_1, LOW);
  digitalWrite(RELAY_MAG_LOCK_2, LOW);
  digitalWrite(RELAY_MAG_LOCK_3, LOW);
  digitalWrite(RELAY_MAG_LOCK_4, LOW);
  digitalWrite(RELAY_SLAVE_1, LOW);
  digitalWrite(RELAY_SLAVE_2, LOW);
  digitalWrite(RELAY_SLAVE_3, LOW);
  digitalWrite(RELAY_SLAVE_4, LOW);
  digitalWrite(RELAY_MASTER, LOW);
  digitalWrite(CORRECT_LED, LOW);
  digitalWrite(INCORRECT_LED, LOW);

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
			isPushed(push_button_1.getSingleDebouncedPress(), (uint8_t) 1);
			isPushed(push_button_2.getSingleDebouncedPress(), (uint8_t) 2);	
			isPushed(push_button_3.getSingleDebouncedPress(), (uint8_t) 3);
			isPushed(push_button_4.getSingleDebouncedPress(), (uint8_t) 4);
			if (counter>=6) state = 1;
		break;
		case 1:
			correct = true;
      state = 2;
			for(int i = 0; i<6; i++){
				Serial.print(seq_tempo[i]);
				Serial.print(" ,");
				if(seq[i]!=seq_tempo[i]){
					correct = false;
					return;
				}
			}
			
		break;
		case 2:
			if(correct){
				digitalWrite(RELAY_MAG_LOCK_1, HIGH);
				delay(100);
				digitalWrite(RELAY_MAG_LOCK_2, HIGH);
				delay(100);
				digitalWrite(RELAY_MAG_LOCK_3, HIGH);
				delay(100);
				digitalWrite(RELAY_MAG_LOCK_4, HIGH);
				delay(100);
				digitalWrite(RELAY_SLAVE_1, HIGH);
				delay(100);
				digitalWrite(RELAY_SLAVE_2, HIGH);
				delay(100);
				digitalWrite(RELAY_SLAVE_3, HIGH);
				delay(100);
				digitalWrite(RELAY_SLAVE_4, HIGH);
				delay(100);
				digitalWrite(RELAY_MASTER, HIGH);
				delay(100);
				digitalWrite(CORRECT_LED, HIGH);
				state = 3;
			}
			else{
				digitalWrite(INCORRECT_LED, HIGH);
				state = 4;
			}
	break;
		case 3:
			if(time_correct_now>=time_correct){
				digitalWrite(CORRECT_LED, LOW);
				state = 5;
			}
		break;
		case 4:
			delay(3000);
			digitalWrite(RELAY_MAG_LOCK_1, LOW);
			digitalWrite(RELAY_MAG_LOCK_2, LOW);
			digitalWrite(RELAY_MAG_LOCK_3, LOW);
			digitalWrite(RELAY_MAG_LOCK_4, LOW);
			digitalWrite(RELAY_SLAVE_1, LOW);
			digitalWrite(RELAY_SLAVE_2, LOW);
			digitalWrite(RELAY_SLAVE_3, LOW);
			digitalWrite(RELAY_SLAVE_4, LOW);
			digitalWrite(RELAY_MASTER, LOW);
			digitalWrite(CORRECT_LED, LOW);
			digitalWrite(INCORRECT_LED, LOW);
      time_elapsed = 0;
			state = 0;	
      counter = 0;
		break;
		case 5:
			
		break;				
	}

	if(millis() > time_now + period){
		if(counter>=1){
			time_elapsed++;
			if((time_elapsed>=time_for_checking)&&(state==0)){
				state = 1;
				time_elapsed = 0;
				counter = 0;
			}
		}
		if(state==3) time_correct_now++;
		time_now = millis();
	}
}

/***************************************************************************
 *
 * isPushed Function
 *
 ***************************************************************************/

bool isPushed(bool p, uint8_t num) {
	if (p)
	{
		button_pressed = num;
		seq_tempo[counter] = button_pressed;
    Serial.print(button_pressed);
    Serial.print(" --> ");
    Serial.println(counter);
		counter++;
	}
}
