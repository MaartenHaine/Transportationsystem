#include <DFRobot_HX711.h>
#include<Servo.h>

//import commands app

// booleans:
  bool type1 = false; //De true of false waarde van de 3 types wordt bepaald door de app input
  bool type2 = false;
  bool type3 = true;
  bool fase1 = true; //bonensilo selectie
  bool fase2 = false; //translatie systeem richting bonen
  bool fase3 = false; //zoeken container
  bool fase4 = false; //translatie systeem richting container
  bool fase5 = false; //afgave bonen
  bool fase6 = false; //reset system

//Definiëring van de 4 (servo)motoren: 
      Servo servo_rotatie;      //draaiing schijf
      Servo servo_translatie;   //verplaatsing volledige systeem
      Servo servo_bakje;        //aansturing voor het bakje

//Definiëring gewichtssensor:
  DFRobot_HX711 MyScale(2, 3); //pinnen nog te bepalen

//Invoering grenswaarde gewicht (invoer via app!!)
  int border_weight = 50;


void setup() {
  Serial.begin(9600);
  
  //De pinnen voor de servomotoren (tijdelijk)
  servo_rotatie.attach(2);
  servo_translatie.attach(3);
  servo_bakje.attach(5);
}



void loop() {
  if (fase1){
    if (type1){
      //draaien naar links servomotor_rotatie
      servo_rotatie.write(130); //exacte waarde nog te bepalen
      
      fase1 = false;
      fase2 = true;
      type1 = false;
    }
    else if (type2){
      fase1 = false;
      fase2 = true;
      type2 = false;
    }
    else if (type3){
      //draaien naar rechts servomotor_rotatie
      servo_rotatie.write(50); //exacte waarde nog te bepalen
      
      fase1 = false;
      fase2 = true;
      type3 = false;
    }
  }
  if (fase2) {
    //Translatie van systeem via servomotor_translatie
    
    //rotatie rolband via DC motor
    
    //activatie meten van de weegschaal
    
    if (MyScale.readWeight() >= 0.98* border_weight){
      //Terugkeren van de kar via servomotor_translatie
      fase2 = false;
      fase3 = true;
    }
  }
  if (fase3) {
    //activatie ultrasoonsensor
    //kleine rotatie van de grondplaat
    //als sensor iets vind dat kleiner is dan 10 cm, turn other side
    //als sensor iets vind dat tussen 14cm en 40 cm zit, container lock
      //fase3 = false
      //fase4 = true
  }
  if (fase4) {
    
  }
}
