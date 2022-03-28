#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TimerOne.h>
#include <SimpleSleep.h>


#define PWM 6
#define BPR 10
#define BPM 11
#define TG 2

float Vbatt;
float Vled;
float Iled;
float Ialim;
float I=0,erreur=0,ki=10,Ic=0.4,Integral=150;
int count=0,p=0,p2=0;
int m=1,n=1;
int8_t speed=127;
SimpleSleep Sleep;




void setup() {  
    Serial.begin(19200);
pinMode(PWM,OUTPUT);
pinMode(BPR,INPUT_PULLUP);
pinMode(BPM,INPUT_PULLUP);
pinMode(TG,OUTPUT);
TCCR0B = (TCCR0B & 0b11111000) | 0x01; //64kHz Timer 0
analogReference(INTERNAL);// ref 1.1V
analogWrite(PWM,255);

}


void loop(){

I=analogRead(A3);
I=I/465;       //1.1V/(1024*0.5ohms)    mesure du courant max 2.2A


Vbatt=analogRead(A0);       //1.1*4.2/1024
Vbatt=Vbatt*0.00451;







  if (digitalRead(BPM) ==LOW ) {
      p++;
if (p>=20){
  
  m++;                  //incrementation des modes 
  if (m>3) m=1; 

  switch (m) {
  case 1:Ic=0.4;   break;   
  case 2:Ic=0.2;   break;
  case 3:Ic=0.1;   break;
     }
     p=0;
}

  } 
  if (digitalRead(BPR) == LOW  ) {
p2++;
if (p2>=20){
  n++;                  //incrementation des modes 
  if (n>2) n=1; 
 p2=0;
  }
 }
  digitalWrite(TG, !digitalRead(TG));

switch (n){

case 1:    count++;
if (count==0){
    analogWrite(PWM,255); 
  // Sleep.lightlyFor(950);
count++;
}
if (count>0 || count <230){
      analogWrite(PWM,255);
  count++;
}

if (count >=230 && count <250){
  erreur=Ic-I;
Integral= Integral+(erreur*ki); 
if (Integral<=1)Integral=1;
if (Integral>200)Integral=200;
speed=255-Integral;
    analogWrite(PWM,speed);
}
if(count>249){
    count=0;
    

  }  Serial.println(speed);
Serial.println(I);
Serial.println(speed);

 break;

case 2:
erreur=Ic-I;
Integral= Integral+(erreur*ki); 
if (Integral<=1)Integral=1;
if (Integral>186)Integral=186;
speed=255-Integral;
analogWrite(PWM,speed);
Serial.print(I,3);Serial.print(",");
Serial.print(Ic,3);Serial.print(",");
Serial.print(speed);Serial.print(",");
Serial.print(erreur,3);Serial.print(",");
Serial.println(m);
break;


}
digitalWrite(2, !digitalRead(2));
}


//2.5ms
