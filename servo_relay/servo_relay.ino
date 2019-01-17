#include "TimerOne.h"
#include "ServoTimer2.h"
#include "PinChangeInterrupt.h"

typedef void(*VoidFuncPtr)(void);

class PulseMeasure {
  public:
    PulseMeasure(byte _meas_pin, VoidFuncPtr change_dispatch) {
      meas_pin = _meas_pin;
      pinMode(meas_pin, INPUT);

      level = LOW;
      level_old = LOW;
      start_time = 0;
      width = 0;

      attachPinChangeInterrupt(digitalPinToPCINT(meas_pin), change_dispatch, CHANGE);
    }

    unsigned long GetWidth() {
      return  width;
    }

    void change() {
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

  private:
    byte meas_pin;
    bool level;
    bool level_old;
    unsigned long  start_time;
    unsigned long  width;
};

void change0(void);
void change1(void);
void change2(void);
void change3(void);
void change4(void);
void change5(void);
void change6(void);
void change7(void);

PulseMeasure meas[] = {{3, change0}, {4, change1}, {5, change2}, {6, change3}, {7, change4}, {8, change5}, {9, change6}, {10, change7}};
byte servo_pins[] = {12, 13, A5, A4, A3, A2, A1, A0};

void change0(void)
{
  meas[0].change();
}

void change1(void)
{
  meas[1].change();
}

void change2(void)
{
  meas[2].change();
}

void change3(void)
{
  meas[3].change();
}

void change4(void)
{
  meas[4].change();
}

void change5(void)
{
  meas[5].change();
}

void change6(void)
{
  meas[6].change();
}

void change7(void)
{
  meas[7].change();
}

ServoTimer2 servo[8];

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < sizeof(servo) / sizeof(servo[0]); i++)  {
    servo[i].attach(servo_pins[i]);
  }
}

int incPulse(int val, int inc) {
  if ( val + inc  > 2000)
    return 1000;
  else
    return val + inc;
}

void loop() {
  static int pulse[8] = {1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700};

  for (int i = 0; i < sizeof(servo) / sizeof(servo[0]); i++ ) {
    servo[i].write(pulse[i]);
  }
  delay(1000);

  for (int i = 0; i < sizeof(meas) / sizeof(meas[0]); i++) {
    Serial.print(pulse[i]);
    Serial.print("/");
    Serial.print(meas[i].GetWidth());
    Serial.print("\t");
  }
  Serial.println("");

  for (int i = 0; i < sizeof(servo) / sizeof(servo[0]); i++)  {
    pulse[i] = incPulse(pulse[i], 100);
  }
}

