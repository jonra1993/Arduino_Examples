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
#include <SPI.h>
#include <MFRC522.h>
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>

//! Pinout definition
#define SIGNAL 5
#define RST_PIN	9   
#define SS_PIN	10  

//! Creates RC522 object
MFRC522 mfrc522(SS_PIN, RST_PIN); 

//! DFPlayer object
DFRobotDFPlayerMini player;

//! variables
volatile int volumen=28; //Set volume value (0~30).
int song_number = 1;

//! UID allowed
byte tag[4]= {0xA9, 0x3F, 0xE1, 0x20}; 
byte actual_UID[4];   //temporal UID 
/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {
  Serial.begin(9600);

	SPI.begin();        //Iniciamos el Bus SPI
	mfrc522.PCD_Init(); // Iniciamos el MFRC522

  if (!player.begin(Serial)) {
    //while(!player.begin(Serial));
	}
  player.volume(volumen);  //Set volume value (0~30).
  player.EQ(DFPLAYER_EQ_NORMAL);

  pinMode(SIGNAL, OUTPUT);
  digitalWrite(SIGNAL, HIGH);
  delay(1000);
  player.loop(song_number);
}

/***************************************************************************
 *
 * Loop Function
 *
 ***************************************************************************/
void loop() {
	//! Is there any new card?
	if ( mfrc522.PICC_IsNewCardPresent()) {  
    if ( mfrc522.PICC_ReadCardSerial()) {
      //! Extract its UID
      for (byte i = 0; i < mfrc522.uid.size; i++) { 
        actual_UID[i]=mfrc522.uid.uidByte[i];          
      }                
      //Compare actual UID with tag
      if(compareArray(actual_UID,tag)){
        player.stop(); 
        digitalWrite(SIGNAL, LOW);
      }
      //! Finish communication wirh RFID
      mfrc522.PICC_HaltA();
    }   
	}
}

/***************************************************************************
 *
 * compareArray Function
 *
 ***************************************************************************/
//! Compare UID RFID with the tag registed
 boolean compareArray(byte array1[],byte array2[])
{
  if(array1[0] != array2[0])return(false);
  if(array1[1] != array2[1])return(false);
  if(array1[2] != array2[2])return(false);
  if(array1[3] != array2[3])return(false);
  return(true);
}
