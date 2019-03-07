int ek, ek_1, ek_2;
float uk_1;
int pwm_foco=3;
int pwm_motor=5;
int pwm=0;
int adc=0;

byte x=0;

  float kp=5;
  float ki=2;
  float kd=0;
  float T=0.2;
  int setpoint;
  int temperatura;

 volatile char cadena[4];   //LONGITUD MAXIMA DE LA CADENA DE CARACTERES ES 100
 int comenzar=0;
 int unidad; 
 int decena; 
 int centena; 

 
void setup() {
  analogReference(INTERNAL);
  Serial.begin(9600);

 cadena[0]='0'; 
 cadena[1]='3';
 cadena[2]='0';  
}

void loop() {

  temperatura=map(analogRead(adc),0,1023,2,110);   
  temperatura=temperatura-4; 
  if (comenzar==1)
  {
  PID(temperatura, setpoint, kp,ki,kd, &ek_1, &ek_2, &uk_1, T );
  motor_f(uk_1); 
  foco_f(uk_1); 
  }
  delay(300);
  
}

float PID(int setpoint, int medida, float Kp, float Ki, float Kd,int *ek_1, int *ek_2,float *uk_1, float T){ //T en segundos
      
  float uk, t0, t1, q0, q1, q2, c;  //SeÃ±al de control
  int ek,a,b;

  ek=medida-setpoint;

 
  t0=Kd/T;
  t1=Ki*T;
    
  q0=Kp+t0;
  q1=-Kp-2*t0+t1;
  q2=Kp*t0;
  //ley de control
  a=*ek_1;
  b=*ek_2;
  c=*uk_1;
  
  uk=c+q0*ek+q1*a+q2*b;
  if(uk>255) uk=255;
  if(uk<-255) uk=-255;
  //aplica uk
  //Actualiza datos
  *uk_1=uk;
  *ek_1=ek;
  *ek_2=a;
  return 0;
  }

void motor_f(float leydecontrol)
{
   
  if (leydecontrol<0)
  {
    pwm=int(leydecontrol);
    if(pwm<-255) pwm=-255;    
    analogWrite(pwm_motor,-pwm);
    }
  else 
  {
    analogWrite(pwm_motor,-pwm);
    }
}

void foco_f(float leydecontrol)
{
  if(leydecontrol>0)
  {
    pwm=int(leydecontrol);
    if(pwm>255) pwm=255;
    analogWrite(pwm_foco,pwm);
   }
  else 
  {
    analogWrite(pwm_foco,0);
   }
}

void serialEvent() {

  //se debe enviar la cada ejemplo 005 020  100
  if (Serial.available()) {
    // get the new byte: 
    char inChar = (char)Serial.read();
    
    if (inChar==10)   //SI EL DATO DE ENTRADA ES ENTER
    {
      x=0;                 //se resetea el contador 
      centena=(cadena[0]-48)*100; //configurar led R 
      decena=(cadena[1]-48)*10; //configurar led G
      unidad=(cadena[2]-48)*1; //configurar led B
      setpoint=centena+decena+unidad;    
      comenzar=1;
      Serial.print("El nuevo setpoint de temperatura es: ");
      Serial.println(setpoint);  
      
     }
 
    else
    {
      if (inChar>=48 && inChar<=57)
      {
      cadena[x++]=inChar;   //SE GUARDA EL DATO EN UNA POSICION DEL VECTOR
      if (x==3) x=0;
      }
      else     
      {
        if (inChar=='a')
          {
            Serial.print("La temperatura actual es :");
            Serial.print(temperatura);
            Serial.println(" C");  
          }

       else if (inChar=='s')
          {
             Serial.print("El valor de ley de control es: ");
             Serial.println(uk_1);
          }
        else Serial.println("Temperatura erronea vuelva a ingresar el setpoint");

      }
    }
  }
}
  


  

