#include<Servo.h>
//#include<Definitie_File_definities.ino>
Servo servo_translatie;
Servo servo_rotatie;

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

void setup() {
  servo_rotatie.attach();
  servo_translatie.attach();
  
  pinMode(10, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(24, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  float afstandtotbakjes = 0; //nog te bepalen
  bool rolbandisbijbakjes = false;
  bool bakjelinks = false; //ligt eraan welk bakje we moeten gebruiken
  bool bakjerechts = false;

}

void loop() {
  
    
}
  

void servo(int pos){
  pos =(180*pos)/270;
  servo_translatie.write(pos);
}

void continuous(int pin, int hoek) {
  servo_rotatie.attach(pin);
  servo_rotatie.write(hoek);
}

void search(){
  //IN EEN SNELLE LUS!!!!
  // Clears the trigPin condition
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   // Reads the echoPin, returns the sound wave travel time in microseconds
   duration = pulseIn(echoPin, HIGH);
   // Calculating the distance
   distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
   // Displays the distance on the Serial Monitor
   //Serial.print("Distance: ");
   //Serial.print(distance);
   //Serial.println(" cm");
   bool rolbandisbijbakjes = rolbandisbijbakjes;
   bool bakjelinks = bakjelinks; 
   bool bakjerechts = bakjerechts;
   float afstandtotbakjes = afstandtotbakjes;
   if (distance == afstandtotbakjes) {
     bool rolbandisbijbakjes = true;
   }
   if (rolbandisbijbakjes == true) {
     if (bakjelinks = true) {
       //draai rolband naar links
     }
     else if (bakjerechts = true) {
       //draai rolband naar rechts
     }
   
   } 
}
