#include <Servo.h> 
Servo servomotor; 

float Kp =0.2; 
float Kd = 10; 
float Ki =0; 

unsigned long time = 0; 
int periodo = 50; 

int angulo=0;

volatile int error_anterior;
volatile int error; 
volatile int distancia;
volatile int setpoint=205;
volatile float I; // Valor Integral
volatile float D; // Valor derivativo


const int echo = 3;
const int trigger = 4;
const int serv= 7;



float u;
float angulo_horizontal = 130; 
volatile uint8_t comenzar=0;


void setup() 
{ 
  servomotor.attach(serv); 
  Serial.begin(9600);
  pinMode(trigger, OUTPUT); 
  pinMode(echo, INPUT); 
  servomotor.write(angulo_horizontal);
  delay(1000);
 
} 


void loop() 
{
  if(comenzar==1){
     
  if ((millis()>time+periodo)){ 
    time = millis();


    error_anterior = error; 
    int d=funcion_para_medir_distancia(trigger, echo);
    if(d<0)
    {
      distancia=0; 
    }
    else
    {
      if(d>1000) d=0; 
      distancia = d;
    }
    
    error=setpoint-distancia;

    D= (error - error_anterior)/periodo;
   
    
    I=I+error;
    
    u=Kp*error+Kd*D+Ki*I;
    
    if (u<-50) u=-50;
    if (u>50) u=50;
    
    angulo=angulo_horizontal+u; 
    angulo=constrain(angulo,100,160); //se restringe el valoR
    servomotor.write(angulo);
    delay(50); 
  }
  }
}

int funcion_para_medir_distancia(int TriggerPin, int EchoPin) {   
  
   float dist;
   
  digitalWrite(trigger,LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigger,LOW); 
  
  dist = float(pulseIn(echo, HIGH)/2 * 0.0343)-1;
  dist=int (dist*10);
  return dist;
}

void serialEvent() {
  if (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar=='q')
    {
      if(comenzar==0) comenzar=1;
      else comenzar=0;
    }
    
    else if(inChar=='d'){ 
      Serial.print("error: ");
      Serial.println(error);
      Serial.print("setpoint: ");
      Serial.println(setpoint);
      Serial.print("[mm]");  
      Serial.print("distancia:: ");
      Serial.println(distancia);    
    }
   }
 
 }


