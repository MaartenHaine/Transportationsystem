
/*
   Deze demo werkt samen met de Android applicatie die jullie hebben gekregen. Via een wificonnectie tussen smartphone en ESP32 is er communicatie mogelijk tussen Arduino en androidtoestel in beide richtingen.
   In deze demo worden via een app commando's met parameters (#ledjes) doorgestuurd naar de arduino waarin de commando's worden ontleed. Ook zend Arduino informatie terug naar Android (hoeveel ledjes er branden).
   Daarmee zitten al de communicatiemogelijkheden die jullie nodig gaan hebben in het project, verwerkt in dit voorbeeld.

   Je hebt ook een functie die de 16 servo kanalen kan aansturen gekregen. Deze kan ook aangestuurd worden vanuit de demo-app.

   Je kan de commando's die je ontvangt en stuurt via wifi aflezen via de Seriële monitor! Open die zeker om het beter te begrijpen!
*/


// *****************************BELANGRIJK **********************************

/* Als je je Arduino code wilt testen zonder ArduinoShield, zet de boolean withWifi dan op False! Je kan dan in de Seriële monitor de commando's ingeven die je normaal via de app
zou versturen. Dit is ideaal om je hele Arduino code te debuggen. De monitor print ook uit wat hij normaal via Wifi naar de app zou sturen. Zo kan je dus perfect zowel je app
als je Arduino code al maken zonder Shield!

Bijvoorbeeld als je de demo test kan je in de seriele monitor SERVO/12150 ingeven om servo nummer 12 in een hoek van 150 graden te zetten.

*/

boolean withWifi = true; //Test your Arduino code with Serial Monitor, set on True to test with the App en Arduinoshield.

// ***************************************************************

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


//********************************BELANGRIJK***************************

///  Dit hangt af van het type servomotor! Deze waardes zijn voor de MG90's die 180° bereik heeft. 
#define SERVOMIN  80 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  409 // This is the 'maximum' pulse length count (out of 4096)

// Afhankelijk van het type servo dat je gebruikt zal je hier zelf je eigen min en max moeten definiëren. Check ook de functie servocontrol

//****************************

int aantal = 0; // start met geen enkel ledje aan
int startLEDPin = 30; // LED's zijn D30 t.e.m. D38 dus start op pin 30

// ---------------------------------------------------- setup() en loop () ---------------------------------------------------------

void setup() {
  
  // NIET AANPASSEN ------------------------
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  
  
  pinMode(25, OUTPUT);
  digitalWrite(25,HIGH); 
  
  Serial.begin(9600);
  delay(100);
  Serial2.begin(115200); // Communication with Wifi module
  
  // ------------------------- Vanaf hier mag je aanpassen voor je eigen programma

  //Zet al de ledjes uit van D30 t.e.m 
  for (int i = 1; i < 8; i++) {
    pinMode(startLEDPin + i, OUTPUT);
    digitalWrite(startLEDPin+i, 0);
  }
}

void loop() {
 //Kijk of er iets werd verzonden over wifi?
  String command = checkwifi();
  //Voer het commando uit als er iets werd ontvangen
  if (command.length()>0){
    Serial.println("Ontvangen commando: " + command); // Print het commando (debug)
    //Splits het command op in commando/parameters
    String commando = command.substring(0,command.indexOf('/'));
    String parameters = command.substring(command.indexOf('/')+1);
    doCommand(commando,parameters);
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


// ---------------------------------- PAS HIERONDER AAN WELKE COMMANDO's JE ZELF WILT ONTVANGEN --------------------------------------------------------

void doCommand(String commando, String message) {
  //welk commando werd ontvangen?
  if (commando.equals("LED")) 
  {
    // welke parameter werd nog meegestuurd?
    ledControl(message.toInt());
  }
  else if (commando.equals("SERVO"))
  {
    //Welke parameter werd nog meegestuurd?
    servoControl(message.toInt());
  }
  else {
    sendWifi("Commando niet herkend");
  }
}

// -------------------------------------VOEG HIER JE EIGEN FUNCTIES TOE -------------------------------------------------------------
// ---------------------------------------------------- Demo om ledjes aan te zetten op het shield of de servomotoren aan te sturen ---------------------------------------------------------

void ledControl(int aantalLEDS) {
  // zet het gewenste aantal LED's aan/uit
  for (int i = 0; i <= aantalLEDS - 1; i++) {
    digitalWrite(startLEDPin + i, 1);
  }
  // zet al de ledjes met nummers > aantalLEDS sowieso uit
  for (int i = aantalLEDS; i < 8; i++) {
    digitalWrite(startLEDPin + i, 0);
  }
  //hoeveel lichtjes branden er en zend dit door via wifi naar
  sendWifi("LED/" + String(aantalLEDS));
  delay(5);
}

void servoControl(int message) {
 // message = number XXYYY with XX servonumber and YYY value
  int servoNumber = message/1000; //XX servonumber 
  int servoPWM = message%1000; //YYY cornervalue between 0 and 256
  servoPWM = map(servoPWM, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(15-servoNumber, 0, servoPWM);
}
