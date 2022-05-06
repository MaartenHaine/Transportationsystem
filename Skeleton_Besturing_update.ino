#include <DFRobot_HX711.h>
#include<Servo.h>

//import commands app


//gewichtssensor
#define WAIT

#include <limits.h>
#include <HX711.h>

HX711 weight;

#ifdef WAIT
long minv,maxv,total;
#endif


//Pins
int rotpin = ;
int buttonPin = ;

int red_light_pin= ;
int green_light_pin = ;
int blue_light_pin = ;

int dcmotorPin = ;
int relaisPin = ;


//Voor functies
Servo servo_translatie;
Servo servo_rotatie;

#define echoPin 4 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 5 //attach pin D3 Arduino to pin Trig of HC-SR04

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

//Om te testen
bool weightreached = false;

//WIFI MODULE STUFF
/* Als je je Arduino code wilt testen zonder ArduinoShield, zet de boolean withWifi dan op False! Je kan dan in de Seriële monitor de commando's ingeven die je normaal via de app
zou versturen. Dit is ideaal om je hele Arduino code te debuggen. De monitor print ook uit wat hij normaal via Wifi naar de app zou sturen. Zo kan je dus perfect zowel je app

als je Arduino code al maken zonder Shield!

Bijvoorbeeld als je de demo test kan je in de seriele monitor SERVO/12150 ingeven om servo nummer 12 in een hoek van 150 graden te zetten.

*/

boolean withWifi = false; //Test your Arduino code with Serial Monitor, set on True to test with the App en Arduinoshield.

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
  bool type3 = false;
  bool fase1 = true; //bonensilo selectie
  bool fase2 = false; //translatie systeem richting bonen
  bool fase3 = false; //zoeken container
  bool fase4 = false; //translatie systeem richting container
  bool fase5 = false; //afgave bonen
  bool fase6 = false; //reset system


//Invoering grenswaarde gewicht (invoer via app!!)
  int border_weight = 50;


void setup() {
  
  
  //pins
  pinMode(dcmotorPin, OUTPUT);
  pinMode(relaisPin, OUTPUT);
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  
  servo_rotatie.attach(2);
  servo_translatie.attach(3);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  pinMode(10, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(24, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  float afstandtotbakjes = 0; //nog te bepalen
  bool rolbandisbijbakjes = false;
  bool bakjelinks = false; //ligt eraan welk bakje we moeten gebruiken
  bool bakjerechts = false;
  delay(100)
    
  //gewichtssensor
  weight.begin(2,3);
  weight.set_offset(216630.48);
  weight.set_scale(2216.24);
}



void loop() {
  //Kijk of er iets werd verzonden over wifi?
  String command = checkwifi();
  //Voer het commando uit als er iets werd ontvangen
  if (command.length()>0){
    Serial.println("Ontvangen commando: " + command); // Print het commando (debug)
    
    if (fase1) {
      kalibratie();
      RGB_color(255, 0, 0);
      if(type1){
        int fase1tijd = 100; //deze is de hoek voor de fase 1 continuous draaiing
        continuous(rotpin, 65);
        delay(fase1tijd);
        continuous(rotpin, 90);
        gewicht = type1gewicht
        type1 = false;
        fase1 = false;
        fase2 = true;
      }
      else if(type2) {
        int fase2tijd = 100; //deze is de hoek voor de fase 2 continuous draaiing
        continuous(rotpin, 65);
        delay(fase2tijd);
        continuous(rotpin, 90);
        gewicht = type2gewicht
        type2 = false;
        fase1 = false;
        fase2 = true;
      }
      else if(type3) {
        int fase3tijd = 100; //deze is de hoek voor de fase 2 continuous draaiing
        continuous(rotpin, 65);
        delay(fase3tijd);
        continuous(rotpin, 90);
        gewicht = type3gewicht
        type3 = false;
        fase1 = false;
        fase2 = true;
      }
    }
    if (fase2) {
      //Translatie van systeem naar voren via servomotor_translatie
      int vooruithoek = 100; //hoek voor vooruitbeweging
      servo270(vooruithoek);
      delay(100); //wachten opdat systeem vooruit is bewogen
      //rotatie rolband via DC motor
      
      
      
      speed = 100; //snelheid rotatie rolband van 0-255
      DCmotor(true, speed);
      //activatie meten van de weegschaal
      while (!weightreached) {
        Serial.println(weight.get_units(1));
        if (weight.get_units(1) <= gewicht) {
          RGB_color(0, 255, 0);
          DCmotor(false, speed);
          delay(2000);
          DCmotor(true, 0);
        
          fase2 = false;
          fase3 = true;
        }
      }
    }
    
    if (fase3) {
      kalibratie();
      int middenachteruithoek = 100; //hoek voor achteruitbeweging translatie
      servo270(middenachteruithoek);
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
      int middenachteruithoek = 100;
      servo270(middenachteruithoek);
      fase4 = false;
      fase5 = true;
    }
    if (fase5) {
    //Translatie bakje 
      bakjetijd = 1000;
      continuous(bakjecontpin, 65);
      delay(bakjetijd);
      continuous(bakjecontpin, 90);
      delay(500)
      RGB_color(255, 0, 0);
      fase5 = false;
      fase6 = true;
    }
    if (fase6) {
      continuous(bakjecontpin, 110);
      delay(bakjetijd);
      continous(bakjecontpin, 90);
      delay(500);
      fase6 = false;
      fase1 = true;
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
void kalibratie() {
  bool buttonState = false;
  statement_rotatieknop = true;
  while(statement_rotatieknop) {
    buttonState = digitalRead(buttonPin);
    continuous(rotpin, 65);
    delay(1000)
    if (buttonState == HIGH) {
      continuous(rotpin, 90)
      statement_rotatieknop = false;
      return true;    
    }
  }
}
 

void servo270(int pos){
  pos =(180*pos)/270;
  servo_translatie.write(pos);
}

void continuous(int pin, int hoek) {
  servo_rotatie.attach(pin);
  servo_rotatie.write(hoek);
}


void search(int right = 0){
  //IN EEN SNELLE LUS!!!!
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  int distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  //Serial.print("Distance: ")
  //Serial.print(distance)
  //Serial.println(" cm")
  if (distance <= 42 and distance >= 10) {
    return true;
    return right;
  }
  else if (right >= 0 and right < 180){ //right wordt gebruikt om huidige positie te definiëren
    continuous(100 ,85); // eerste deel moet nog aangepast worden
    delay(10);
    continuous(100 ,90); // eerste deel moet nog aangepast worden
    right += 5;
  }
  return false;
  return right;
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, 255-red_light_value);
  analogWrite(green_light_pin, 255-green_light_value);
  analogWrite(blue_light_pin, 255-blue_light_value);
}

void DCmotor(bool richting, int speed) { 
  if (richting) { //rotatie rolband normaal
    digitalWrite(relaisPin, LOW);
    analogWrite(dcmotorPin, speed);
  }
  else { //omegekeerde richting
    digitalWrite(relaisPin, HIGH);
    analogWrite(dcmotorPin, speed);
  }
}
