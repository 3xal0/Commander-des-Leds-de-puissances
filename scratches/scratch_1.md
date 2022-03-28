## PARTIE 2 : REGULATION EN BOUCLE FERMEE
####Objectif : Réguler la LED en fonction de la tension de batterie et Consommer le moins possible.


Le principe est de récuperer sur une sortie analogique la tension délivrée est de la convertir en intensité. Ensuite on fait varier le rapport cyclique de la PWM de sorte à se rapprocher d'une consigne défini par les différents modes de puissances.

- Ic -> consigne en intensité

- Integral -> correction à appliquer sur le rapport cyclique de la PWM

- Ki -> Facteur de vitesse d'asservissement (il permet de déterminé la vitesse à laquelle on régule. Attention plus ce facteur est grand plus faible est la précision)

- erreur -> Différence entre la consigne et le courant.

Concernant le mode de veille, on se rend vite compte que les interruptions ne sont pas simple d'utilisation lors ce qu'on met en place un Sleep mode. Cela est due au fait qu'il faut gérer les paramètres de mise en veille. Nous avons donc choisi la faciliter en revsistant le programme:
Il utilise maintenant aucune interruption car nous utilisons comme Timer le temps d'une loop.

On obtient :
```dart
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

I=analogRead(A3); //récupération de la tension avant conversion
I=I/465;       //1.1V/(1024*0.5ohms)    mesure du courant max 2.2A


Vbatt=analogRead(A0);       //1.1*4.2/1024
Vbatt=Vbatt*0.00451;







  if (digitalRead(BPM) ==LOW ) {
      p++;
if (p>=20){
  
  m++;                  //incrementation des modes 
  if (m>3) m=1; 

  switch (m) { // Différentes consignes de régulation
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
   Sleep.lightlyFor(950);// Mise en place d'un mode veille permettant un moins haute conso durant les phase éteintes
count++;
}
if (count>0 || count <230){
      analogWrite(PWM,255);
  count++;
}

if (count >=230 && count <250){
  erreur=Ic-I; //Calcul de l'erreur par rapport à la consigne
Integral= Integral+(erreur*ki); // Intregral correspond à la correction à apporter
if (Integral<=1)Integral=1;//Limite de la PWM avant arrêt
if (Integral>200)Integral=200;// Limite de la PWM pour ne pas avoir un courant trop important
speed=255-Integral;// Correction du rapport cyclique
    analogWrite(PWM,speed);
}
if(count>249){
    count=0;
    

  }  Serial.println(speed);
Serial.println(I);
Serial.println(speed);

 break;

case 2:
erreur=Ic-I;// Ici même régulation mais en continu
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


//4 ms


```

Résultat sur le montage réel :

![img_2.png](img_2.png)