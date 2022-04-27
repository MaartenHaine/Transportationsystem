#include<Servo.h>
Servo servo_translatie;
int transleren;


void setup() {
  servo_translatie.attach(4);
  Serial.begin(9600);
}


void loop() {
  if (Serial.parseInt() == 1) {
    if (transleren == true){
      transleren = false;
    }
    else {
      transleren = true;
    }
    Serial.println(transleren);
    delay(10);
  }
  
  if (transleren == true){
    servo_translatie.write(180);
    delay(10);
  }
  if (transleren == false){
    servo_translatie.write(0);
    delay(10);
  }
}
  









 
