
/************************************************************************************
 *
 * File Name: Hall_Sec.ino 
 * Developer: Jonathan Vargas 
 * Web: www.jonathanvargas.ml
 * Date: 16/04/2019
 * Description:  Hall sequence and leds
 * 
 ***********************************************************************************/

/******************************************************************************
	Copy your files to an empty SD card
	Connect the Serial MP3 Player to the Arduino board
	GND → GND
	VCC → 5V
	TX → pin 5
	RX → pin 6
 *******************************************************************************/
/******************************************************************************
	Hall sensor and LEDs connections
  Hall → pin A0
  LED1 → pin 7
  LED2 → pin 8
  LED3 → pin 9
  LED4 → pin 10
 *******************************************************************************/

//! Libraries included
#include "Arduino.h"
#include <SoftwareSerial.h>

//! Pinout definitions
#define ARDUINO_RX 5  //should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6  //connect to RX of the module
#define HALL_SENSOR 0  

//! Debug Definitions
#define DEBUG true              // true for printing DEBUG info

//! Pinout definitions
const int maxLeds = 4;
const int LEDS[maxLeds] = { 7, 8, 9, 10};

     
//! Variables
volatile byte state = 0;
volatile uint8_t counter = 0;
const uint8_t numberSeq = 3;
bool hallState = false;    // current state of the hall sensor
bool lastHallState = false; // previous state of the hall sensor
long startDetection = 0L;   
long endDetection = 0L;      
long timeHold = 0L;        // the time in which hall sensor detected magnet
long timeReleased = 0L;    // the time in which hall sensor did not detect magnet
bool musicOn = false;

//! Threshold for analog hall sensor
int minThreshold = 0;
int maxThreshold = 500;

//! secuence
int valid_time_interval = 2 ;  // time in which user should keep magnet

//!  Serial Objects for connecting catalex
SoftwareSerial catalex(ARDUINO_RX, ARDUINO_TX);//init the serial protocol, tell to catalex wich pins are TX and RX

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
	#if DEBUG == true
		// Initialize hardware serial port (serial debug port)
		Serial.begin(9600);
		// Wait for serial port to connect. Needed for native USB port only
		while (!Serial);
		Serial.println("");
		Serial.println("==============================");
		Serial.println("==== Hall sequence Debug ===");
		Serial.println("==============================");
	#endif

	// Leds Initialization
	for (int i = 0; i < maxLeds; i++) {
		pinMode(LEDS[i],OUTPUT);
    digitalWrite(LEDS[i], LOW);
  }

	// Catalex init
  catalex.begin(9600);//Start our Serial coms for THE MP3
  delay(500);
  SendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card  
  delay(200);	

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
    	// Read hall state
      static int hallAnalog = 0;
      hallAnalog = analogRead(HALL_SENSOR);
      hallState = ValidateMeasure(hallAnalog, maxThreshold, minThreshold);
      if (hallState != lastHallState) { // hall state changed
        updateState(hallState, startDetection, endDetection, timeHold, timeReleased, valid_time_interval);
      }
      lastHallState = hallState;
      if(counter >= numberSeq){
        state = 3;
      }
      break;
    case 1:
      Sequence1();
      break; 
    case 2:
      Sequence2();
      break;
    case 3:
      Sequence3();
      break;
    default:
      break; 
  }

}

/***************************************************************************
 *
 *  Sequence1 Function 
 * 
 * Description: led sequence
 *
 ***************************************************************************/
void Sequence1()
{
  #if DEBUG == true
    Serial.println("Led Sequence 1: ");
  #endif
	// Leds Initialization
	for (int i = 0; i < maxLeds; i++) {
    digitalWrite(LEDS[i], HIGH);
    delay(500);
    digitalWrite(LEDS[i], LOW);
  }
}
/***************************************************************************
 *
 *  Sequence2 Function 
 * 
 * Description: led sequence
 *
 ***************************************************************************/
void Sequence2()
{
  #if DEBUG == true
    Serial.println("Led Sequence 2: ");
  #endif
	for (int i = 0; i < maxLeds; i++) {
    digitalWrite(LEDS[i], HIGH);
  }
  delay(500);
  for (int i = 0; i < maxLeds; i++) {
    digitalWrite(LEDS[i], LOW);
  }
  delay(500);
}

/***************************************************************************
 *
 *  Sequence3 Function 
 * 
 * Description: led sequence
 *
 ***************************************************************************/
void Sequence3()
{
  #if DEBUG == true
    Serial.println("Led Sequence 3: ");
  #endif
  digitalWrite(LEDS[0], HIGH);
  digitalWrite(LEDS[1], LOW);
  digitalWrite(LEDS[2], HIGH);
  digitalWrite(LEDS[3], LOW);
  delay(500);
  digitalWrite(LEDS[0], LOW);
  digitalWrite(LEDS[1], HIGH);
  digitalWrite(LEDS[2], LOW);
  digitalWrite(LEDS[3], HIGH);
  delay(500);
}

/***************************************************************************
 *
 * ValidateMeasure Function 
 * 
 * Description: Return true id mesure is between max and min limits
 *
 ***************************************************************************/
bool ValidateMeasure(int measure, int max, int min)
{
  bool state = false;
  if((measure>min) && (measure<max)){
    state = true;
  }
  // // #if DEBUG == true
  // //   Serial.print("State: ");
  // //   Serial.print(state);
  // //   Serial.print(" --> Hall Value: ");
  // //   Serial.print(measure);
  // //   Serial.print(" --> Max Limit: ");
  // //   Serial.print(max);
  // //   Serial.print(" --> Min Limit: ");
  // //   Serial.println(min);
  // // #endif
  return state;
}

/***************************************************************************
 *
 * SendCommand Function 
 * 
 * Description: Catalex commands
 *
 ***************************************************************************/
void SendCommand(int8_t command, int16_t dat)
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
   catalex.write(Send_buf[i]) ;//send bit to serial mp3
   //Serial.print(Send_buf[i],HEX);//send bit to serial monitor in pc
 }
 delay(1000);
 //Serial.println();
}

/***************************************************************************
 *
 * updateState Function 
 * 
 * Description: 
 *
 ***************************************************************************/
void updateState( int hallState, 
                  long &startDetection, 
                  long &endDetection, 
                  long &timeHold, 
                  long &timeReleased,
                  int validTime) {

  // the magnet was just located
  if (hallState == true) {
    startDetection = millis();
    timeReleased = startDetection - endDetection;
  	if(musicOn==false){
      SendCommand(CMD_PLAY_WITHVOLUME, 0x1E01);
      musicOn = true;
		} 
    #if DEBUG == true
      Serial.print("Sensor was idle for ");
      Serial.print(timeReleased);
      Serial.println(" [ms]");
    #endif
  }
  // the magnet was just released 
  else {
    endDetection = millis();
    timeHold = endDetection - startDetection;
    musicOn = false;
		SendCommand(STOP_PLAY,0X0000);

    if(timeHold>(long)(validTime*1000)) counter++;
    else counter = 0;

    #if DEBUG == true
      Serial.print("Counter ");
      Serial.println(counter);
    #endif
    #if DEBUG == true
      Serial.print("Magnet was hold for ");
      Serial.print(timeHold);
      Serial.println(" [ms]");
    #endif
  }
}

