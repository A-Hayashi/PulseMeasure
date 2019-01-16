#include "TimerOne.h"
#include "ServoTimer2.h"

class PulseMeasure {
  public:
    PulseMeasure(byte _meas_pin) {
      meas_pin = _meas_pin;
      pinMode(meas_pin, INPUT);

      level = LOW;
      level_old = LOW;
      start_time = 0;
      width = 0;
    }

    void CyclicHandler() {
      level = digitalRead(meas_pin);

      if (level_old != level) {
        if (level == HIGH) {
          start_time = micros();
        } else {
          width = micros() - start_time;
        }
      }
      level_old = level;
    }

    unsigned long GetWidth() {
      return  width;
    }

    byte meas_pin;
    bool level;
    bool level_old;
    unsigned long  start_time;
    unsigned long  width;
};

PulseMeasure meas[] = {3, 4, 5, 6, 7, 8, 9, 10};
ServoTimer2 servo;

void timerFire() {
  for (int i = 0; i < sizeof(meas) / sizeof(meas[0]); i++) {
    meas[i].CyclicHandler();
  }
}

void setup() {
  Timer1.initialize(80);
  Timer1.attachInterrupt(timerFire);
  Serial.begin(9600);

  servo.attach(2);
}

int incPulse(int val, int inc) {
  if ( val + inc  > 2000 )
    return 1000 ;
  else
    return val + inc;
}

void loop() {
  static int pulse = 1000;

  servo.write(pulse);
  delay(1000);

  Serial.print("Servo:");
  Serial.print(pulse);
  Serial.print("\tMeasure:");
  for (int i = 0; i < sizeof(meas) / sizeof(meas[0]); i++) {
    Serial.print(meas[i].GetWidth());
    Serial.print("/");
  }
  Serial.println("");

  pulse = incPulse(pulse, 200);
}

