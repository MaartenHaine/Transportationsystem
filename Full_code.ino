
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
int rotpin = 13;
int bakjepin = 11;
int buttonPin = 42;

int red_light_pin= 44;
int green_light_pin = 45;
int blue_light_pin = 46;

int dcmotorPin = 12;
int relaisPin = 9;




//Voor functies
Servo servo_translatie;
Servo servo_rotatie;

#define echoPin 24      //4 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10       //5 //attach pin D3 Arduino to pin Trig of HC-SR04

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

///  Dit hangt af van het type servomotor! Deze waardes zijn voor de MG90's die 180° bereik heeft. 
#define SERVOMIN  80 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  409 // This is the 'maximum' pulse length count (out of 4096)

// Afhankelijk van het type servo dat je gebruikt zal je hier zelf je eigen min en max moeten definiëren. Check ook de functie servocontrol

int aantal = 0; // start met geen enkel ledje aan
int startLEDPin = 30; // LED's zijn D30 t.e.m. D38 dus start op pin 30

//////////////////////////////////////////////////////////////////////////////////////////

// booleans:
  bool type1 = false; //De true of false waarde van de 3 types wordt bepaald door de app input
  bool type2 = false;
  bool type3 = false;
  bool fase1 = false; //bonensilo selectie
  bool fase2 = false; //translatie systeem richting bonen
  bool fase3 = false; //zoeken container
  bool fase4 = false; //translatie systeem richting container
  bool fase5 = false; //afgave bonen
  bool fase6 = false; //reset system


//Invoering grenswaarde gewicht (invoer via app!!)
  int border_weight = 50;

  int type1gewicht = 50;
  int type2gewicht = 45;
  int type3gewicht = 60;
  int gewicht = 0;


void setup() {
  // put your setup code here, to run once:
  
    // NIET AANPASSEN ------------------------
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  
  
  pinMode(25, OUTPUT);
  digitalWrite(25,HIGH); 
  
  Serial.begin(9600);
  delay(100);
  Serial2.begin(115200); // Communication with Wifi module
  
  //////////////////////////////::
  //pins
  pinMode(dcmotorPin, OUTPUT);
  pinMode(relaisPin, OUTPUT);
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  

  servo_translatie.attach(4);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  pinMode(10, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(24, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  float afstandtotbakjes = 0; //nog te bepalen
  bool rolbandisbijbakjes = false;
  bool bakjelinks = false; //ligt eraan welk bakje we moeten gebruiken
  bool bakjerechts = false;
  delay(100);
    
  //gewichtssensor
  weight.begin(2,3);
  weight.set_offset(216630.48);
  weight.set_scale(2216.24);
  

}

void loop() {
  //Check of er een wifi bericht is.
  String command = checkwifi();
  //Voer het commando uit als er iets werd ontvangen
  if (command.length()>0){
    Serial.println("Ontvangen commando: " + command); // Print het commando (debug)
    //Splits het command op in commando/parameters
    int slash1 = command.indexOf('/');
    int slash2 = command.indexOf('/', slash1 + 1);
    int slash3 = command.indexOf('/', slash2 + 1);

    //nemen types hun gewichten
    String commando = command.substring(0,slash1);
    String STRtype1gew = command.substring(slash1+1, slash2);
    String STRtype2gew = command.substring(slash2+1, slash3);
    String STRtype3gew = command.substring(slash3+1);

    //omzetten string naar int
    int type1gewicht = STRtype1gew.toInt();
    int type2gewicht = STRtype2gew.toInt();
    int type3gewicht = STRtype3gew.toInt();

    //Types check
    if (type1gewicht > 0) {
      type1 = true;
      fase1 = true;
    }
    if (type2gewicht > 0) {
      type2 = true;
      fase1 = true;
    }
    if (type3gewicht > 0) {
      type3 = true;
      fase1 = true;
    }
  }
  
  if (fase1) {
    RGB_color(255, 0, 0);
    kalibratie();
    if(type1){
      int fase1tijd = 100; //deze is de hoek voor de fase 1 cont draaiing.
      continuous(rotpin, 65);
      delay(fase1tijd);
      continuous(rotpin, 90);
      gewicht = type1gewicht;
      type1 = false;
      fase1 = false;
      fase2 = true;
    }
    else if(type2) {
      int fase2tijd = 100; //deze is de hoek voor de fase 2 continuous draaiing
      continuous(rotpin, 65);
      delay(fase2tijd);
      continuous(rotpin, 90);
      gewicht = type2gewicht;
      type2 = false;
      fase1 = false;
      fase2 = true;
    }
    else if(type3) {
      int fase3tijd = 100; //deze is de hoek voor de fase 2 continuous draaiing
      continuous(rotpin, 65);
      delay(fase3tijd);
      continuous(rotpin, 90);
      gewicht = type3gewicht; //moet nog geconvert worden
      type3 = false;
      fase1 = false;
      fase2 = true;
    }
  }
  if(fase2) {
    //translatie systeem naar voren via servomotor
    int vooruithoek = 100; //hoek voor vooruitbeweging
    servo270(vooruithoek);
    delay(100); //wachten opdat systeem vooruit is bewogen

    
    //rotatie rolband via DC motor
    int snelheid = 160; //snelheid rotatie DC
    DCmotor(true, snelheid);

    //activatie meten van de weegschaal
    while(!weightreached) {
      Serial.println(weight.get_units(1));
      if (weight.get_units(1) <= gewicht) {
        RGB_color(0, 255, 0);
        DCmotor(false, snelheid);
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
    int bakjetijd1 = 1000;
    
    continuous(bakjepin, 65);
    delay(bakjetijd1);
    continuous(bakjepin, 90);
    delay(500);
    
    RGB_color(255, 0, 0);
    fase5 = false;
    fase6 = true;
  }
  if (fase6) {
    int bakjetijd2 = 1000;
    continuous(bakjepin, 110);
    delay(bakjetijd2);
    continuous(bakjepin, 90);
    delay(500);
    fase6 = false;
    fase1 = true;
    
  }
}
// ===================================================
//functies
// ===================================================
//WIFI
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

//=====================================
// BEWEGINGEN MOTOREN
//=====================================
void kalibratie() {
  bool buttonState = false;
  bool statement_rotatieknop = true;
  while(statement_rotatieknop) {
    buttonState = digitalRead(buttonPin);
    continuous(rotpin, 65);
    delay(1000);
    if (buttonState == HIGH) {
      continuous(rotpin, 90);
      statement_rotatieknop = false;  
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


// voorbeeld
void servoControl(int message) {
 // message = number XXYYY with XX servonumber and YYY value
  int servoNumber = message/1000; //XX servonumber 
  int servoPWM = message%1000; //YYY cornervalue between 0 and 256
  servoPWM = map(servoPWM, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(15-servoNumber, 0, servoPWM);
}

//=====================================
// SENSOREN
//=====================================
bool search(){
  int right = 0; 
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
  }
  else if (right >= 0 and right < 180){ //right wordt gebruikt om huidige positie te definiëren
    continuous(100 ,85); // eerste deel moet nog aangepast worden
    delay(10);
    continuous(100 ,90); // eerste deel moet nog aangepast worden
    right += 5;
  }
  return false;
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, 255-red_light_value);
  analogWrite(green_light_pin, 255-green_light_value);
  analogWrite(blue_light_pin, 255-blue_light_value);
}
