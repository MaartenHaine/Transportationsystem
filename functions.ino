#include<Servo.h>
//#include<Definitie_File_definities.ino>
Servo servo_translatie;
Servo servo_rotatie;


void setup() {
  servo_rotatie.attach(3);
  servo_translatie.attach(4);
  Serial.begin(9600);

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

void 
