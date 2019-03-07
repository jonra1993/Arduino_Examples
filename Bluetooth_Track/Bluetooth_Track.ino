#include <TimerOne.h>
#include <EEPROM.h>
#include "pitches.h"

volatile int address_cancion = 0;
volatile int address_R = 1;
volatile int address_G = 2;
volatile int address_B = 3;

//EEPROM.write(address_cancion, 11); direccion y valor
byte value;

//VARIABLES DE LECTURA
volatile int val_cancion;
volatile int val_R;
volatile int val_G;
volatile int val_B;

//declaraci[on de pines
volatile char cadena[10];   //LONGITUD MAXIMA DE LA CADENA DE CARACTERES ES 100
const int parlante=11;
const int pulsador=2;
const int led=13;
const int linterna=12;
const int pinRed=3;
const int pinGreen=5;
const int pinBlue=6;
const int vibrador=10;

double lapso=100000; //microsegundos 

//auxiliares
bool tempo;
bool n;
bool i;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

boolean me_estan_buscando=false;
boolean encender_lin=false;
boolean encender_rgb=false;
boolean on=false;

byte x=0;
int cuanto_pulsado;
int g;
int cuento_tiempo;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////  AUDIO
int numTones = 15;
int estrofa1[] = { NOTE_G4,NOTE_A4,NOTE_G4,NOTE_E4,NOTE_G4,NOTE_A4,NOTE_G4,NOTE_E4,NOTE_D5,NOTE_D5,
                 NOTE_B5,NOTE_B5,NOTE_C5,NOTE_C5,NOTE_G4};

int estrofa2[] = { NOTE_A5,NOTE_A5,NOTE_A5,NOTE_C5,NOTE_B5,NOTE_A5,NOTE_G4,NOTE_A5,NOTE_G4,NOTE_E4,
                   NOTE_A5,NOTE_A5,NOTE_A5,NOTE_C5,NOTE_B5,NOTE_A5,NOTE_G4,NOTE_A5,NOTE_G4,NOTE_E4};
                   
int estrofa3[] = { NOTE_D5,NOTE_D5,NOTE_F5,NOTE_D5,NOTE_B5,NOTE_C5,NOTE_E5,NOTE_C5,NOTE_G4,NOTE_E4,NOTE_G4,
                   NOTE_F4,NOTE_D4,NOTE_C4};

int dragon[] = { NOTE_CS5,NOTE_CS5,NOTE_A4,NOTE_B4,NOTE_CS5,NOTE_B5,NOTE_CS5,NOTE_B4,NOTE_A4,NOTE_G4,NOTE_A4,
                 NOTE_A4,NOTE_F4,NOTE_G4,NOTE_A4,NOTE_B4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_E4,NOTE_D4,NOTE_D4,NOTE_F4,
                 NOTE_DS4,NOTE_CS4,NOTE_F4,NOTE_F4,NOTE_G4,NOTE_A4,NOTE_B4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_G4,NOTE_F4,
                 NOTE_E4,NOTE_F4};

long noteDurations1[] = {1, 4, 2, 1 ,1, 4 , 2, 1, 1 ,2, 1,2,1,2,1};

long noteDurations2[] = {1,4,2,1,4,2,1,4,2,1,1,4,2,1,4,2,1,4,2,1};

long noteDurations3[] = {1,4,1,4,2,1,1,1,4,2,1,4,2,1};

long noteDurations4[] = {2,2,4,4,4,4,2,2,2,2,2,2,4,4,4,4,2,2,2,2,2,3,2,4,3,1,1,4,2,2,4,4,4,4,2,2,2,4,4,4,4,4};

long noteDuration;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


void setup() 
{
  pinMode(pulsador,INPUT_PULLUP);
  attachInterrupt(0, pulso, FALLING);
  pinMode(led,OUTPUT);
  pinMode(linterna,OUTPUT);
  pinMode(vibrador,OUTPUT); 
  
  conftimer();
  Serial.begin(9600);


  setColor(10,100,255);
  val_cancion=EEPROM.read(address_cancion);
  val_R=EEPROM.read(address_R);
  val_G=EEPROM.read(address_G);
  val_B=EEPROM.read(address_B);

}

void loop() 
{
  if (encender_rgb==true) setColor(val_R,val_G,val_B);
  else setColor(0,0,0);

  if (encender_lin==true) digitalWrite(linterna,HIGH);
  else digitalWrite(linterna,LOW);

  if (me_estan_buscando=true) funcion_buscar();
 
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////   Funcion de buscar
void funcion_buscar()
{
  
}
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////   PULSADOR

void pulso()
{
  delay(10);
  tempo=digitalRead(pulsador);
  if(tempo==LOW)
  {
    g++;
    if(g>2)
    {
      if(encender_lin==false) encender_lin=true;
      else encender_lin=false;
      g=0;      
    }
    
    if ((me_estan_buscando=true)||(encender_rgb==true))
    {
      me_estan_buscando=false;
      encender_rgb=false;    
    }   
  } 
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//// CONFIGURAR TIMER

void conftimer()
{
  Timer1.initialize(lapso);  //esta en microsegundos
  Timer1.attachInterrupt(ya1ms); // activa interrupcion del timer1
}

void ya1ms(void) //esta calibrado para 100ms
{
  cuento_tiempo++;
  if(cuento_tiempo>5) cuento_tiempo=0;
   
   if (digitalRead(pulsador)==LOW) cuanto_pulsado++;
   else cuanto_pulsado=0;

   if(cuanto_pulsado>8) g=0;
   
   if(cuanto_pulsado>20)
   {
      cuanto_pulsado=0;
      Serial.print('x'); //buscar celular
   }
   if (me_estan_buscando=true)
   {
       if (cuento_tiempo==0)
       {
          if (on==false)
          {
            on=true;
            setColor(val_R,val_G,val_B);
            digitalWrite(vibrador,HIGH);
          }
          else
          {
            on=false;
            setColor(0,0,0);
            digitalWrite(vibrador,LOW); 
          }         
       }
   }   
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///   CONFIGURAR LED RGB

void setColor(int red, int green, int blue)
{ 
  green=map(green,0,255,255,0);
  analogWrite(pinBlue, 255-blue); 
  analogWrite(pinRed, 255-red);
  analogWrite(pinGreen, green);
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/// DATOS RECIBIDOS DE COMUNICACIÒN SERIAL

void serialEvent() {
  if (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    if (inChar=='0')   //SI EL DATO DE ENTRADA ES ENTER
    {
      cadena[x++]='\0';  //indica fin de cadena
      x=0;                 //se resetea el contador

      if(cadena[0]=='1')    //inicia la comunicacion
        {
          if(cadena[1]=='c')  //configuracion
          {
            if(cadena[2]=='c')  //cancion
              {
                 switch(cadena[3])
                 {
                 case 'a':
                    val_cancion=0; //configurar cancion 0
                    EEPROM.write(address_cancion, val_cancion); 
                 break;
                 case 'b':
                    val_cancion=1; //configurar cancion 1
                    EEPROM.write(address_cancion, val_cancion); 
                 break;
                 case 'c':
                    val_cancion=2; //configurar cancion 2
                    EEPROM.write(address_cancion, val_cancion);   
                 break;
                 case 'd':
                    val_cancion=3; //configurar cancion 3
                    EEPROM.write(address_cancion, val_cancion);   
                 break;          
                }  
              }      
           if(cadena[2]=='l') //led rgb
             {
                  val_R=cadena[3]; //configurar led R 
                  val_G=cadena[4]; //configurar led G
                  val_B=cadena[5]; //configurar led B 
                  EEPROM.write(address_B, val_B);
                  EEPROM.write(address_R, val_R);
                  EEPROM.write(address_B, val_B);
             } 
           
          }
         else if(cadena[1]=='a')  //realizar accion
         {
            switch(cadena[2])
            {
             case 'a':  //encender linterna
                encender_lin=true;
                
             break;
             case 'b': //apagar linterna
                encender_lin=false;
             break;
             case 'c':  //encender rgb
                encender_rgb=true;
             break;
             case 'd':  //apagar rgb
               encender_rgb=false;                             
             break;
             case 'e': //buscar objeto perdido
                me_estan_buscando=true;
             break;
             case 'f': //ya no buscar objeto perdido
                me_estan_buscando=false;
             break;                                
            }              
         }          
          
     }        
            
    }
    else
    {
      cadena[x++]=inChar;   //SE GUARDA EL DATO EN UNA POSICION DEL VECTOR
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/// CANCION DRAGON BALL

void dragonball()
{   
      for (int i = 0; i < 21; i++)
  {
   
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations4[i];
    tone(parlante, dragon[i],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/// CANCIÒN NOCHE DE PAZ

void noche()
{ 
  
  for (int i = 0; i < numTones; i++)
  {
   
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations1[i];
    tone(parlante, estrofa1[i],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }
  delay(500);
  
  for (int i = 0; i < 20; i++)
  {
   
    int noteDuration = 1000/noteDurations2[i];
    tone(parlante, estrofa2[i],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }
  delay(500);
  
  for (int i = 0; i < 14; i++)
  {
   
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations3[i];
    tone(parlante, estrofa3[i],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }  

}
