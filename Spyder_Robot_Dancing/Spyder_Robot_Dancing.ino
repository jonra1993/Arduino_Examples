#include <TimerOne.h>
#include <Servo.h>

#define timer 10000



#define motor1 13
#define motor2 12
#define pwm 11

#define s1 10
#define s2 9
#define s3 8
#define s4 7
#define s5 6
#define s6 5
#define s7 4

#define pulsador 3

#define mov_frente 0
#define mov_derecha 1
#define mov_izquierda 2
#define mov_atras 3
#define mov_paro 4


int posinicial[7];
int posfinal[7];
int contador[7];
int rapidez[7];

Servo myservo1; 
Servo myservo2; 
Servo myservo3; 
Servo myservo4; 
Servo myservo5; 
Servo myservo6; 
Servo myservo7;  

int comenzar=0;

void setup() {
  myservo1.attach(s1);  // attaches the servo on pin s1 to the servo object
  myservo2.attach(s2);
  myservo3.attach(s3);
  myservo4.attach(s4);
  myservo5.attach(s5);
  myservo6.attach(s6);
  myservo7.attach(s7);


  //inicializa el timer
  Timer1.initialize(timer); // set a timer of length 10000 microseconds 
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here

  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(pulsador, INPUT_PULLUP);

  attachInterrupt(1, pulso, FALLING);

  Serial.begin(9600);
  
  do {

  }while(comenzar==0);

  
}

void loop()
{

  mov_servo(1, 0, 180, 1); //mueve el servo 1 de 0 a 180 a una velocidad de 1
  motordc(mov_frente, 50);
  //delay(2000);
  while(true)
  {
  }

}


void pulso()
{
  delay(20);  //antirebotes
  if (digitalRead(pulsador)==LOW)
  {
    comenzar=1;
  }  
}

void mov_servo(int s, int ini, int fin, int cuanrapido)
{
  if (ini<0) ini=0;
  if (ini>255) ini=255;
  if (fin<0) fin=0;
  if (fin>255) fin=255;
    
  switch(s)
  {
    case 1:
     posinicial[0]=ini;
     contador[0]=ini;
     posfinal[0]=fin;
     rapidez[0]=cuanrapido;   
    break;
    case 2:
     posinicial[1]=ini;
     contador[1]=ini;
     posfinal[1]=fin;  
     rapidez[1]=cuanrapido;
    break;
    case 3:
     posinicial[2]=ini;
     contador[2]=ini;
     posfinal[2]=fin;   
     rapidez[2]=cuanrapido;
    break;
    case 4:
     posinicial[3]=ini;
     contador[3]=ini;
     posfinal[3]=fin;  
     rapidez[3]=cuanrapido;
    break;            
    case 5:
     posinicial[4]=ini;
     contador[4]=ini;
     posfinal[4]=fin;
     rapidez[4]=cuanrapido;
    break;    
    case 6:
     posinicial[5]=ini;
     contador[5]=ini;
     posfinal[5]=fin;  
     rapidez[5]=cuanrapido;
    break;            
    case 7:
     posinicial[6]=ini;
     contador[6]=ini;
     posfinal[6]=fin;
     rapidez[6]=cuanrapido;
    break;    
  }  
}


void timerIsr()
{
  for (int i=0; i<7; i++)
  {
   if(posfinal[i]>posinicial[i])
    {
      contador[i]=contador[i]+rapidez[i];
      if (contador[i]>posfinal[i])
      {
        contador[i]=posfinal[i];
        posinicial[i]=posfinal[i];
      }      
    }  
   else if (posfinal[i]<posinicial[i])
    {
      contador[i]=contador[i]-rapidez[i];
      if (contador[i]<posfinal[i])
      {
        contador[i]=posfinal[i];
        posinicial[i]=posfinal[i];
      }    
    }  
   else //no hace nada
      {
      }    
  }

  myservo1.write(contador[0]);
  myservo1.write(contador[1]);
  myservo1.write(contador[2]);
  myservo1.write(contador[3]);
  myservo1.write(contador[4]);
  myservo1.write(contador[5]);
  myservo1.write(contador[6]);
      
}

void motordc(int mov, int vel) //vel de 0 al 100%
{
  
  int velocidad= map(vel, 0, 100, 0, 255);
  analogWrite(pwm,velocidad);
  switch(mov)
  {
    case 0:
      digitalWrite(motor1,HIGH);
      digitalWrite(motor2,HIGH);
    break;
    case 1:
      digitalWrite(motor1,HIGH);
      digitalWrite(motor2,LOW);    
    break;
    case 2:
      digitalWrite(motor1,LOW);
      digitalWrite(motor2,HIGH);    
    break;
    case 3:
      digitalWrite(motor1,LOW);
      digitalWrite(motor2,LOW);    
    break;            
    case 4:
    analogWrite(pwm,0);
    break;    
  }
  
}
