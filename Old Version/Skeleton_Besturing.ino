#include <DFRobot_HX711.h>
#include<Servo.h>

//import commands app

//Pins
int contpin = ;


//WIFI MODULE STUFF
/* Als je je Arduino code wilt testen zonder ArduinoShield, zet de boolean withWifi dan op False! Je kan dan in de Seriële monitor de commando's ingeven die je normaal via de app
zou versturen. Dit is ideaal om je hele Arduino code te debuggen. De monitor print ook uit wat hij normaal via Wifi naar de app zou sturen. Zo kan je dus perfect zowel je app

als je Arduino code al maken zonder Shield!

Bijvoorbeeld als je de demo test kan je in de seriele monitor SERVO/12150 ingeven om servo nummer 12 in een hoek van 150 graden te zetten.

*/

boolean withWifi = true; //Test your Arduino code with Serial Monitor, set on True to test with the App en Arduinoshield.

// NIET AANPASSEN, start commando's voor de wifimodule en servomotors.
String startsequence = "CMDS/";
String stopsequence = "/CMDEND/";

String startsequenceSetup = "SETUPB/";
String stopsequenceSetup = "/SETUPE/";

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
// *************************

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
  //Kijk of er iets werd verzonden over wifi?
  String command = checkwifi();
  //Voer het commando uit als er iets werd ontvangen
  if (command.length()>0){
    Serial.println("Ontvangen commando: " + command); // Print het commando (debug)
    
    if (fase1) {
      if(type1){
        fase1tijd = ; //deze is de hoek voor de fase 1 continuous draaiing
        continous(contpin, 65);
        delay(fase1tijd);
        continous(contpin, 90);
        type1 = false;
        fase1 = false;
        fase2 = true;
      }
      else if(type2) {
        type2 = false;
        fase1 = false;
        fase2 = true;
      }
      else if(type3) {
        fase2tijd = ; //deze is de hoek voor de fase 1 continuous draaiing
        continous(contpin, 115);
        delay(fase2tijd);
        continous(contpin, 90);
        type3 = false;
        fase1 = false;
        fase2 = true;
      }
    }
    if (fase2) {
      //Translatie van systeem naar voren via servomotor_translatie
      vooruithoek = ; //hoek voor vooruitbeweging
      servo270(vooruithoek);
      delay(...) //wachten opdat systeem vooruit is bewogen
      //rotatie rolband via DC motor
      
      //activatie meten van de weegschaal
      
      if (weightreached) {
        
        middenachteruithoek = ; //hoek voor achteruitbeweging translatie
        servo270(middenachteruithoek)
        fase2 = false;
        fase3 = true;
      }
      
    }
    if (fase3) {
      if (search()) {
    //activatie ultrasoonsensor
    //kleine rotatie van de grondplaat
    //als sensor iets vind dat kleiner is dan 10 cm, turn other side
    //als sensor iets vind dat tussen 14cm en 40 cm zit, container lock
        fase3 = false;
        fase4 = true;
      }
    }
    if (fase4) {
      //Translatie van systeem naar achteren via servomotor_translatie
      middenachteruithoek = ;
      servo270(middenachteruithoek)
      fase4 = false;
      fase5 = true;
    }
    if (fase5) {
    //Translatie bakje 
      bakjeachteruit = 270;
        
      servo270(bakjeachteruit);
      fase5 = false;
      fase6 = true;
    }
    if (fase6) {
      
    }
  }
    
  }
    
    
  }
  

  

  
  if (fase6) {
    //Terugdraaien naar standaardpositie
    
    fase6 = false
  }
}


// ---------------------------------------------------- Functies voor de connectie met ESP32; NIET AANPASSEN AUB ---------------------------------------------------------

String checkwifi() {
  //Kijkt of er iets werd verzonden over wifi, ontvangt het en decodeert het ook.
  String command = "";
  
  if (withWifi){
    //Read input from ESP32
    while (Serial2.available())
    {
      command = Serial2.readString();
    }
    // Haal de startsequence van het command
    if (command.indexOf(startsequence)>0){
      command = command.substring(command.indexOf(startsequence)+startsequence.length());
      command = command.substring(0,command.indexOf(stopsequence));
    }
    // Is het een setup commando? 
    else if (command.indexOf(startsequenceSetup)>0){
      displayESP32Setup(command);
      command = "";
    }
    else {
      command = "";
    }
  }
  else{
  //Read input from Serial Monitor
     while (Serial.available())
    {
      command = Serial.readString();
    }
  }
  return command;
}

void sendWifi(String message){
  if (withWifi) {
    Serial2.println(message);
  }
  else {
    Serial.print("Message for Wifi: ");
    Serial.println(message);
  }
}

void displayESP32Setup(String command){
  command = command.substring(command.indexOf(startsequenceSetup)+startsequenceSetup.length());
  command = command.substring(0,command.indexOf(stopsequenceSetup));
  Serial.println(command);
}

// ------------------------------------------------------------------------------
// Functies 

