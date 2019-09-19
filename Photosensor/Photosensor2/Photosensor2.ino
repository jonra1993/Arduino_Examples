
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
	TX → pin 5
	RX → pin 6
 *******************************************************************************/


//! Libraries included
#include "Arduino.h"
#include <SoftwareSerial.h>

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
#define ARDUINO_RX 5//should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6//connect to RX of the module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);//init the serial protocol, tell to myserial wich pins are TX and RX
volatile int vol=29;   // from 0 to 30
int song = 1;
////////////////////////////////////////////////////////////////////////////////////
//all the commands needed in the datasheet(http://geekmatic.in.ua/pdf/Catalex_MP3_board.pdf)

static int8_t Send_buf[8] = {0} ;//The MP3 player undestands orders in a 8 int string
                                 //0X7E FF 06 command 00 00 00 EF;(if command =01 next song order) 
#define NEXT_SONG 0X01 
#define PREV_SONG 0X02 
#define CMD_PLAY_W_INDEX 0X03 //DATA IS REQUIRED (number of song)
#define VOLUME_UP_ONE 0X04
#define VOLUME_DOWN_ONE 0X05
#define CMD_SET_VOLUME 0X06//DATA IS REQUIRED (number of volume from 0 up to 30(0x1E))
#define SET_DAC 0X17
#define CMD_PLAY_WITHVOLUME 0X22 //data is needed  0x7E 06 22 00 xx yy EF;(xx volume)(yy number of song)
#define CMD_SEL_DEV 0X09 //SELECT STORAGE DEVICE, DATA IS REQUIRED
#define DEV_TF 0X02 //HELLO,IM THE DATA REQUIRED    
#define SLEEP_MODE_START 0X0A
#define SLEEP_MODE_WAKEUP 0X0B
#define CMD_RESET 0X0C//CHIP RESET
#define CMD_PLAY 0X0D //RESUME PLAYBACK
#define CMD_PAUSE 0X0E //PLAYBACK IS PAUSED
#define CMD_PLAY_WITHFOLDER 0X0F//DATA IS NEEDED, 0x7E 06 0F 00 01 02 EF;(play the song with the directory \01\002xxxxxx.mp3
#define STOP_PLAY 0X16
#define PLAY_FOLDER 0X17// data is needed 0x7E 06 17 00 01 XX EF;(play the 01 folder)(value xx we dont care)
#define SET_CYCLEPLAY 0X19//data is needed 00 start; 01 close
#define SET_DAC 0X17//data is needed 00 start DAC OUTPUT;01 DAC no output

////////////////////////////////////////////////////////////////////////////////////

 
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
  mySerial.begin(9600);//Start our Serial coms for THE MP3
  delay(500);
  sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card  
  delay(200);

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
		if(music_on==false){
			sendCommand(CMD_PLAY_WITHVOLUME, 0x1E01);
			sendCommand(SET_CYCLEPLAY, 0x0000);
		} 
		music_on = true;

	}
	else {
		music_on = false;
		sendCommand(STOP_PLAY,0X0000);

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

void sendCommand(int8_t command, int16_t dat)
{
 delay(20);
 Send_buf[0] = 0x7e; //starting byte
 Send_buf[1] = 0xff; //version
 Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
 Send_buf[3] = command; //
 Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
 Send_buf[5] = (int8_t)(dat >> 8);//datah
 Send_buf[6] = (int8_t)(dat); //datal
 Send_buf[7] = 0xef; //ending byte
 for(uint8_t i=0; i<8; i++)//
 {
   mySerial.write(Send_buf[i]) ;//send bit to serial mp3
   //Serial.print(Send_buf[i],HEX);//send bit to serial monitor in pc
 }
 delay(1000);
 //Serial.println();
}
