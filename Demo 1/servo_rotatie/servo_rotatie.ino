#include<Servo.h>
Servo servo_rotatie;
bool roteren;
bool omdraaien;

void setup() {
  servo_rotatie.attach(3);
  Serial.begin(9600);
  roteren = false;
  omdraaien = false;
}


void loop() {
  if (Serial.parseInt() == 1) {
    if (roteren == true){
      roteren = false;
    }
    else {
      roteren = true;
    }
  }
if (Serial.parseInt() == 2) {
    if (omdraaien == true){
      omdraaien = false;
    }
    else {
      omdraaien = true;
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
  if (omdraaien == true){
    servo_rotatie.write(110);
    delay(10);
  }
}
  
