#include "TimerOne.h"
#include "ServoTimer2.h"

#define SERVO_N 3

byte servo_pins = 2;
byte meas_pins = 5;
int pulse_width;

ServoTimer2 servo;
unsigned long  width;

void timerFire() {
  static bool level_old = LOW;
  static unsigned long  start_time = 0;
  bool level = digitalRead(meas_pins);

  if (level_old != level) {
    if (level == HIGH) {
      //      Serial.println("rise");
      start_time = micros();
    } else {
      //      Serial.println("fall");
      width = micros() - start_time;
    }
  }
  level_old = level;
}

void setup() {
  Timer1.initialize(50);
  Timer1.attachInterrupt(timerFire);
  Serial.begin(9600);

  servo.attach(servo_pins);
  pinMode(meas_pins, INPUT);
}

int incPulse(int val, int inc) {
  if ( val + inc  > 2400 )
    return 500 ;
  else
    return val + inc;
}

void loop() {
  static int pulse = 500;
  servo.write(pulse);
  delay(1000);

  Serial.print("Servo:");
  Serial.print(pulse);
  Serial.print("\tMeasure:");
  Serial.print(width);
  Serial.println("");

  pulse = incPulse(pulse, 100);
}

