#include<Servo.h>
Servo servo_rotatie;
bool roteren;


void setup() {
  servo_rotatie.attach(3);
  Serial.begin(9600);
  roteren = false;
}


void loop() {
  if (Serial.parseInt() == 1) {
    if (roteren == true){
      roteren = false;
    }
    else {
      roteren = true;
    }
    Serial.println(roteren);
    delay(10);
  }
  
  if (roteren == true){
    servo_rotatie.write(70);
    delay(10);
  }
  if (roteren == false){
    servo_rotatie.write(90);
    delay(10);
  }
}
  
