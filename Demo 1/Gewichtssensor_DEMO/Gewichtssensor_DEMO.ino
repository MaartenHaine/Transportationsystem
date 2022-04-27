#define WAIT

#include <limits.h>
#include <HX711.h>

HX711 weight;

#ifdef WAIT
long minv,maxv,total;
#endif
bool vraag = true;

void setup() {
  Serial.begin(9600);
  delay(100);
  weight.begin(2,3);
  weight.set_offset(216630.48);
  weight.set_scale(2216.24);

}

//*
void loop() {
  Serial.println(weight.get_units(1));
  if (vraag)  {
    if (weight.get_units(1) <= 1800) {
      Serial.print("Ok");
      vraag = false;
      }
  else if (Serial.parseInt() == 1) {
    vraag = true;
    }  
    
  }
}
// */
/*
#define SAMPLES 100
// /*
void loop() {
#ifdef WAIT
  if (Serial.available()) {
    while (Serial.available())
      Serial.read();
    Serial.println("measuring...");
    minv = LONG_MAX; maxv = LONG_MIN; total = 0;
    for (int i = 0; i < SAMPLES; i++) {
#endif
      long readv = weight.read();
#ifdef WAIT
      if (readv < minv)
        minv = readv;
      if (readv > maxv)
        maxv = readv;
      total += readv;
#endif
      Serial.println(readv);
#ifdef WAIT
      delay(10);
    }
    Serial.print("min: "); Serial.println(minv);
    Serial.print("max: "); Serial.println(maxv);
    Serial.print("avg: "); Serial.println(total / SAMPLES);
  } else
    delay(200);
#endif
}
// */
