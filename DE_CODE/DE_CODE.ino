
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
int rotpin = 11;
int bakjepin = 7;
int buttonPin = 41;
int systpin = 8;

int red_light_pin= 44;
int green_light_pin = 45;
int blue_light_pin = 46;

int dcmotorPin = 12;
int relaisPin = 9;

int vooruitbakje = 1800;
int achteruitbakje = 2300;

//Voor functies
Servo servo_translatie;
Servo servo_rotatie;

#define echoPin 24      //4 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10       //5 //attach pin D3 Arduino to pin Trig of HC-SR04

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

//Om te testen
bool weightreached = true;

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
  bool ontvangen = false; //boolean voor als er iets is ontvangen
  bool statement = true;

  bool gewichtT1 = false;
  bool gewichtT2 = false;
  bool gewichtT3 = false;


//Invoering grenswaarde gewicht (invoer via app!!)
  int border_weight = 50;
  
  int totaalgewicht = 0;
  //rotatie
  int rotatietijd = 0;
  int silo1naar2tijd = 875;
  int silo1naar3tijd = 1675;
  int rotatiesnelheid = 140;
  
  //translatie
  int translatievooruittijd = 1700;
  int translatievooruitsnelheid = 70;
  int translatieachteruittijd = 1100; 
  int translatieachteruitsnelheid = 100;
  //bakje
  int bakjevooruittijd = 1500;
  int bakjeachteruittijd = 1450;
  int bakjevooruitsnelheid = 160;
  int bakjeachteruitsnelheid = 50;
  String gewichten = "";

  float gewicht;
  int type1gewicht;
  int type2gewicht;
  int type3gewicht;


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
  delay(100);
    
  //gewichtssensor
  weight.begin(3,2);
  weight.set_offset(2327507.6);
  weight.set_scale(2096.7);
}


void loop() {
  //Check of er een wifi bericht is@.
  String command = checkwifi();
  //Voer het commando uit als er iets werd ontvangen
  if (command.length()>0){
    totaalgewicht = 0;
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
    type1gewicht = STRtype1gew.toInt();
    type2gewicht = STRtype2gew.toInt();
    type3gewicht = STRtype3gew.toInt();
    Serial.println(type1gewicht);
    Serial.println(type2gewicht);
    Serial.println(type3gewicht);
    //Types check
    if (type1gewicht > 0) {
      type1 = true;
      fase1 = true;
      gewichten = "GEWICHT/";
      Serial.println(0.1);
    }
    else if (type1gewicht == 0) {
      gewichten = "GEWICHT/0/";
      Serial.println(0.11);
    }
    if (type2gewicht > 0) {
      type2 = true;
      fase1 = true;
      Serial.println(0.2);
    }
    else if (type2gewicht == 0) {
      gewichten = gewichten + "0/";
      Serial.println(0.22);
    }
    if (type3gewicht > 0) {
      type3 = true;
      fase1 = true;
      Serial.println(0.3);
    }
    else if (type3gewicht == 0) {
      gewichten = gewichten + "0/";
      Serial.println(0.32);
    }
  }
  if (fase1) {
      RGB_color(0, 255, 255); //rood
      Serial.println(1);
      if(type1){
        Serial.println(1.05);
        gewicht = float(type1gewicht)*0.98;
        Serial.println(gewicht);
        type1 = false;
        fase1 = false;
        fase2 = true;
        ontvangen = true;
        statement = true;
      
        Serial.println(1.1);
      }
      else if(type2) {
        continuous(rotpin, rotatiesnelheid);
        delay(silo1naar2tijd);
        continuous(rotpin, 85);
        gewicht = float(type2gewicht)*0.98;
        
        type2 = false;
        fase1 = false;
        fase2 = true;
        ontvangen = true;
        statement = true;
        Serial.println(1.2);
      }
      else if(type3) {
        continuous(rotpin, rotatiesnelheid);
        delay(silo1naar3tijd);
        continuous(rotpin, 85);
        
        gewicht = float(type3gewicht)*0.98; 
        type3 = false;
        fase1 = false;
        fase2 = true;
        ontvangen = true;
        statement = true;
        Serial.println(1.3);
      }
      else if (!type1 && !type2 && !type3) {
        sendWifi(gewichten);
      }
  }
  
  if(fase2) {
    
    Serial.println(2.1);
    if (statement) {
      //translatie systeem naar voren via servomotor
      continuous(systpin, translatievooruitsnelheid);
      delay(translatievooruittijd);
      continuous(systpin, 84);
      delay(50); //wachten opdat systeem vooruit is bewogen
      
      //rotatie rolband via DC motor
      int snelheid = 255; //snelheid rotatie DC
      DCmotor(true, snelheid);
      statement = false;
      weightreached = false;
    }
    //activatie meten van de weegschaal
    while(!weightreached) {
      int snelheid = 255; //snelheid rotatie DC
      DCmotor(true, snelheid);
      //Serial.println(weight.get_units(1));
      Serial.println(weight.get_units(1));
      if (weight.get_units(1) >= gewicht) {
        int snelheid = 255;
        DCmotor(false, snelheid);
        continuous(systpin, 90);
        delay(3000);
        DCmotor(false, 0);
        gewichten = gewichten + String(weight.get_units(1));
        gewichten = gewichten + "/";
        totaalgewicht += weight.get_units(1);
        Serial.println(2.3);
        RGB_color(255, 0, 255);
        weightreached = true;

        fase2 = false;
        fase3 = true;
      }
    }
  }
  
  if (fase3) {
    Serial.println(3.1);
    continuous(systpin, translatieachteruitsnelheid);
    delay(translatieachteruittijd);
    continuous(systpin, 89);
    if (search() == 1) { 
      Serial.println(3.2);
      continuous(rotpin , 70);
      delay(1000);
      continuous(rotpin , 90);
      //activatie ultrasoonsensor
      //kleine rotatie van de grondplaat
      //als sensor iets vind dat kleiner is dan 10 cm, turn other side
      //als sensor iets vinsd dat tussen 14cm en 40 cm zit, container lock
      fase3 = false;
      fase4 = true;
    }
  }
  if (fase4) {
    //Translatie bakje 
    Serial.println(4);
    delay(1000);
    continuous(bakjepin, bakjevooruitsnelheid);
    delay(bakjevooruittijd);
    continuous(bakjepin, 89);
    delay(100);
    continuous(bakjepin, bakjeachteruitsnelheid);
    delay(bakjeachteruittijd);
    continuous(bakjepin, 89);
    delay(100);
    
    RGB_color(0, 255, 255);
    fase4 = false;
    fase5 = true;
  }
    if (fase5) {
      Serial.println(6);
      if (kalibratie2() == 1) {
        fase5 = false;
        fase1 = true;
    }
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
void kalibratie1() {
  bool buttonState = false;
  bool statement_rotatieknop = true;
  while(statement_rotatieknop) {
    buttonState = digitalRead(buttonPin);
    continuous(rotpin, 75);
    delay(1000);
    if (buttonState == HIGH) {
      continuous(rotpin, 90);
      statement_rotatieknop = false;  
    }
  }
}
 float kalibratie2(){
  //IN EEN SNELLE LUS!!!!;
  
  
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
  if (distance <= 22 && distance >= 8) {
    continuous(rotpin ,90);
    Serial.println("ok");
    return 1;
  }
  continuous(rotpin , 110); // eerste deel moet nog aangepast worden
  delay(500);
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.println(" cm");
  return 2;
}
/*
void(int pos, int pin){
  pos =(180*pos)/295;
  servo_translate.attach(pin, 400, 2600);
  servo_translatie.write(pos);
}
*/
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
 float search(){
  //IN EEN SNELLE LUS!!!!;
  continuous(rotpin , 60); // eerste deel moet nog aangepast worden
  delay(500);
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
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.println(" cm");
  if (distance <= 10 && distance >= 2) {
    continuous(rotpin ,90);
    Serial.println("ok");
    return 1;
  }
  else {
    return 2;
  }
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, 255-red_light_value);
  analogWrite(green_light_pin, 255-green_light_value);
  analogWrite(blue_light_pin, 255-blue_light_value);
}
