#include <Servo.h> 

//pines de conexion
int echo = 8;
int trigger = 9;
int serv= 10;
int pulsador1=2;
int pulsador2=3;

float Kp =0.8; 
float Kd = 5;
float Ki =0; 


unsigned long time = 0; // tiempo de ejecucion del ultimo ciclo
int period = 200; // Periodo de muestreo en ms

int angulo=0;
int angulo_probe=1220;

volatile int last_error;
volatile int error; 
volatile int distancia;
volatile int setpoint=200; //esta en milimetros
volatile int nvel=5; //  numero de valores de velocidad sobre los que calculamos la media
volatile int v[5];
volatile int vel; // valor medio de las nvel
volatile float I; // Valor Integral
int distancia_tempo=0;


Servo myservo;  // create servo object to control a servo
float uk;
float reposo = 1220; // valor que mantiene la barra horizontal
volatile uint8_t bloqueo=0;

void setup() 
{ 
  myservo.attach(serv); 
  Serial.begin(9600);

  pinMode(trigger, OUTPUT); 
  pinMode(echo, INPUT); 
  pinMode(pulsador1,INPUT_PULLUP);
  pinMode(pulsador2,INPUT_PULLUP);
  
  myservo.writeMicroseconds((int)reposo);
  delay(100);
  myservo.detach();  
} 


void loop() 
{
  if((digitalRead(pulsador1)==LOW)) bloqueo=1;
  
  if((digitalRead(pulsador2)==LOW)) bloqueo=0;
   
    
  if ((bloqueo==1)&&(millis()>time+period)){ 
    time = millis();

    // Medimos error
    last_error = error; // Guardamos el valor anterior de error para calcular la velocidad
    
    do
    {
          distancia_tempo=leer_distancia(trigger, echo); //en mmm 
      
    }while(distancia_tempo>399);
    
    if(distancia_tempo<0)
    {
      distancia=0;  //si  esta tocando el ultrasonico
    }
    else
    {
      if(distancia_tempo>1000) distancia_tempo=0; //vuelve a medir
      distancia = distancia_tempo;
    }
    
    error=setpoint-distancia;

    // Calculo de la media de la VELOCIDAD o derivada
    for (int i=0; i<nvel-1; i++){ // Movemos todas hacia la izq para dejar libre la ultima.
      v[i] =v[i+1];
    }
    v[nvel-1]= (error - last_error); // Ponemos un dato nuevo en la ultima posicion
   
    vel=0;
    for (int i=0; i<nvel; i++){ // Calculamos la media
      vel = vel+ v[i];
    }
    vel = vel/nvel; //se promedia
    
    I=I+error*Ki;


    // ley de control
    uk=Kp*error+Kd*vel+I;
    
    if (uk<-250) uk=-250;
    if (uk>250) uk=250;
    
    angulo=reposo+uk; // 1000 is fully counter-clockwise, 2000 is fully clockwise, and 1500 is in the middle
    if(angulo>1500) angulo=1500;
    if(angulo<1000) angulo=1000;
    myservo.attach(serv);
    myservo.writeMicroseconds((int)angulo);
    delay(50);
    myservo.detach();    
  }
}

int leer_distancia(int TriggerPin, int EchoPin) {   

  long tiempo;
  float erroranceCm;
   
  digitalWrite(trigger,LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(10);
  digitalWrite(trigger,LOW); /* Por cuestión de estabilización del sensor*/
  
  tiempo=pulseIn(echo, HIGH)/2; /*tiempo q se demora en ir */      
  erroranceCm = float(tiempo * 0.0343)-1;
  erroranceCm=int (erroranceCm*10); /*transforma a mm*/
  return erroranceCm;
}

void serialEvent() {
  if (Serial.available()) {

    char leer_dato_entrada = (char)Serial.read();
    
    if (leer_dato_entrada=='p')
    {
      if(bloqueo==0) bloqueo=1;
      else bloqueo=0;
    }
    
    if(leer_dato_entrada=='v'){ // Debug
      Serial.print("error: ");
      Serial.print(error);
      Serial.print("|setpoint: ");
      Serial.print(setpoint);
      Serial.print("[mm]");  
      Serial.print("|distancia:: ");
      Serial.print(distancia);
      Serial.print("[mm]");            
      Serial.print("|uk: ");
      Serial.print(uk);
      Serial.print("|angulo: ");
      Serial.println(angulo);      
    }
   }
 
 }


