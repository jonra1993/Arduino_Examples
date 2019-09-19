
/*************************************************************
 *
 * $Workfile: Photosensor.ino $
 *
 * $Creator: Jonathan Vargas $
 *
 * $Description:  $
 *************************************************************/
/******************************************************************************
Copy the files of "SDcard_example" to an empty SD card
Connect the Serial MP3 Player to the Arduino board
	GND → GND
	VCC → 5V
	TX → pin 11
	RX → pin 10
 *******************************************************************************/


//! Libraries included
#include "Arduino.h"
#include "SerialMP3Player.h"

//! definitions
#define LDR_RELAY A0
#define LDR_MUSIC A1
#define RELAY 5
#define LED 13
//! Thresholds
const int threshold_ldr_relay = 100;
const int threshold_ldr_music = 100;
//! lecures
int analog_ldr_relay = 0;
int analog_ldr_music = 0;
//! Time
long minutes = 10;
long time_setpoint = 60*minutes;
long time_elapsed = 0;
long period = 1000;
unsigned long time_now = 0;
//! variables
bool relay_on = false;
bool music_on = false;
bool mem = false;
//! catalex
#define TX 11
#define RX 10
SerialMP3Player mp3(RX, TX);
volatile int vol=29;   // from 0 to 30
int song = 1;

/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {
	pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY, LOW);
	pinMode(LED,OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
	mp3.begin(9600);        // start mp3-communication
  delay(500);
	mp3.sendCommand(CMD_SEL_DEV, 0, 2);   //select sd-card
	mp3.stop();
  delay(500);

}
/***************************************************************************
 *
 * Loop Function
 *
 ***************************************************************************/
void loop() {

	analog_ldr_relay = analogRead(LDR_RELAY);
	if (analog_ldr_relay > threshold_ldr_relay) {
		if(mem==false){
			digitalWrite(RELAY, HIGH);
			digitalWrite(LED, HIGH);
		}
		relay_on = true;
	}
	else {
		digitalWrite(RELAY, LOW);
		digitalWrite(LED, LOW);
		relay_on = false;
		time_elapsed = 0;
		mem = false;
	}

	analog_ldr_music = analogRead(LDR_MUSIC);
	if ( analog_ldr_music > threshold_ldr_music) {
		if(music_on==false) mp3.playSL(song,vol);
		music_on = true;

	}
	else {
		music_on = false;
		mp3.stop();

	}

	if(millis() > time_now + period){
		Serial.print("Relay LDR: ");
		Serial.print(analog_ldr_relay);
		Serial.print("    Music LDR: ");
    Serial.println(analog_ldr_music);
		if(relay_on==true)time_elapsed++;
		if(time_elapsed>=time_setpoint){
			digitalWrite(RELAY, LOW);
			digitalWrite(LED, LOW);
			mem = true;
		}
		time_now = millis();

	}
}

