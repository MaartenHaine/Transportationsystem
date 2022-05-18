
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
int systbewpin = 4;
int buttonPin = 42;

int red_light_pin= 44;
int green_light_pin = 45;
int blue_light_pin = 46;

int dcmotorPin = 12;
int relaisPin = 9;


bool statement = true;

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
  bool buttonState = false;
  bool statement_rotatieknop = true;
  
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
  weight.begin(3,2);
  weight.set_offset(2323186.957);
  weight.set_scale(219979.);
  

  bool statement = true;

}
int vooruitbakje = 1800;
int achteruitbakje = 2300;

void loop() {
  /*
  if (statement) {
    delay(2000);
    continuous(11, 70);
    delay(2000);
    continuous(11, 90);
    delay(1000);
    continuous(11, 110);
    delay(2000);
    continuous(11, 90);
    delay(1000);
    statement = false;
  }

  
  //rotatie van schijf
  
  int fase1tijd = 1000;
  continuous(4, 110);
  delay(2000);

  
 
  //vooruitbeweging van systeem
  // message = number XXYYY with XX servonumber and YYY value
  int servoNumber = 4; //XX servonumber 
  int servoPWM = 180; //YYY cornervalue between 0 and 256
  servoPWM = map(servoPWM, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(15-servoNumber, 0, servoPWM);
  
  
  //translatie
  if (statement) {
    delay(5000);
    continuous(7,70);
    delay(1300);
    continuous(7,85);
    delay(1000);
  
    continuous(7,98);
    delay(1000);
    continuous(7, 85);
    delay(1000);
    statement = false;
  }
  
  
  //rotatie met kalibratie
  while (statement_rotatieknop) {
    buttonState = digitalRead(buttonPin);
    continuous(11, 70);
    delay(100);
    Serial.println(buttonState);
    if (buttonState == HIGH){
      
      continuous(11, 90);
      statement_rotatieknop = false;
    }
  }
  
  if (statement) {
    continuous(rotpin, 65);
    delay(1000);
    continuous(rotpin, 90);
    delay(1000);
    statement = false;
  }
  
  //translatie bakje
  if (statement) {
    continuous(11, 98);
    delay(vooruitbakje);
    continuous(11, 90);
    delay(100);
    continuous(11, 82);
    delay(achteruitbakje);
    continuous(11, 90);
    delay(100);
    statement = false;
  }
  
  //combinatie trans en rot
  if (statement) {
    delay(3000);
    fase2 = true;
    if (fase2) {
      continuous(11, 65);
      delay(1500);
      continuous(11, 90);
      delay(1000);
      fase2 = false;
      fase3 = true;
    }
    if (fase3) {
      continuous(8, 98);
      delay(vooruitbakje);
      continuous(8, 89);
      delay(100);
      continuous(8, 75);
      delay(achteruitbakje);
      continuous(8, 89);
      delay(100);
      statement = false;
      fase3 = false;
    }
  }
  */
  //DC_motor met bakje en gewichtssensor
  if (statement) {

    //dc
    int snelheid = 255; //snelheid rotatie DC
    DCmotor(true, snelheid);
    gewicht = 30;
    //gewichtssensor
    
    while(!weightreached) {
      Serial.println(weight.get_units(1));
      if (weight.get_units(1) >= gewicht) {
        Serial.println(weight.get_units(1));
        
        DCmotor(false, snelheid);
        delay(2000);
        DCmotor(true, 0);
        weightreached = false;
      }
    }
  }
    /*
    if (fase1) {
      continuous(7, 95);
      delay(1200);
      continuous(7, 85);
      delay(1000);
      fase2 = true;
      fase1 = false;
    }
    if (fase2) {
      continuous(8, 98);
      delay(vooruitbakje);
      continuous(8, 90);
      delay(100);
      continuous(8, 82);
      delay(achteruitbakje);
      continuous(8, 90);
      delay(100);
      fase2 = false;
      statement = false;
    }
  }
  
    int snelheid = 255; //snelheid rotatie DC
    DCmotor(true, snelheid);

    if 
    continuous(11, 98);
    delay(vooruitbakje);
    continuous(11, 90);
    delay(100);
    continuous(11, 82);
    delay(achteruitbakje);
    continuous(11, 90);
    delay(100);
    statement = false;
  }


  
  //rotatie rolband met DC_motor
  int snelheid = 255; //snelheid rotatie DC
  DCmotor(true, snelheid);
  
  delay(2000);
  DCmotor(false, snelheid);
  delay(2000);
  DCmotor(true, 0);
  delay(4000);




  
  //bakje beweging
  if (statement) {
    delay(10000);
    continuous(13,75);
    delay(1600);
    continuous(13,85);
    delay(1000); 
    
    int snelheid = 255; //snelheid rotatie DC
    DCmotor(true, snelheid);
    delay(6000);
    DCmotor(true, 0);
    delay(1000);
    
    continuous(13,95);
    delay(1400);
    continuous(13 ,85);
    delay(1000);
    
    continuous(11, 98);
    delay(vooruitbakje);
    continuous(11, 90);
    delay(100);
    continuous(11, 82);
    delay(achteruitbakje);
    continuous(11, 90);
    delay(100);
    statement = false;
  }
  
  //gewichtssensor
  gewicht = 1800;
  RGB_color(255, 0, 0);
  while(!weightreached) {
    Serial.println(weight.get_units(1));
    if (weight.get_units(1) <= gewicht) {
      RGB_color(0, 255, 0);
    }
   */
  
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

void servo270(int pos, int pin){
  //pos =(180*pos)/270;
  servo_translatie.attach(pin, 400,2600);
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
