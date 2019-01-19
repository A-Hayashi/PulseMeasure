#include <Arduino.h>

#include "TimerOne.h"
#include "ServoTimer2.h"
#include "PinChangeInterrupt.h"
#include "TM1637Display.h"

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3

TM1637Display display(CLK, DIO);

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

PulseMeasure meas[] = {{4, change0}, {5, change1}, {6, change2}, {7, change3}, {8, change4}, {9, change5}, {10, change6}, {11, change7}};
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

  InitSeg();
}

int incPulse(int val, int inc) {
  if ( val + inc  > 2000)
    return 1000;
  else
    return val + inc;
}

void loop() {
  //  static int pulse[8] = {1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700};
  static int pulse[8] = {1150, 1350, 1550, 1750, 1400, 1500, 1600, 1700};
  static int width[8];

  for (int i = 0; i < sizeof(servo) / sizeof(servo[0]); i++ ) {
    servo[i].write(pulse[i]);
  }
  delay(1000);

  for (int i = 0; i < sizeof(meas) / sizeof(meas[0]); i++) {
    width[i] = meas[i].GetWidth();
    Serial.print(pulse[i]);
    Serial.print("/");
    Serial.print(width[i]);
    Serial.print("\t");
  }
  Serial.println("");

  for (int i = 0; i < sizeof(servo) / sizeof(servo[0]); i++)  {
    //    pulse[i] = incPulse(pulse[i], 100);
  }

  Pulse2Digits(width, 4);
}

typedef struct {
  int pmin;
  int pmax;
  int digit;
} p2d_t;


/* パルス幅と7SEGに表示する数字の関係 */
p2d_t p2d[] =
{
  {1000, 1100, 0},
  {1101, 1200, 1},
  {1201, 1300, 2},
  {1301, 1400, 3},
  {1401, 1500, 4},
  {1501, 1600, 5},
  {1601, 1700, 6},
  {1701, 1800, 7},
  {1801, 1900, 8},
  {1901, 2000, 9},
};

/* 複数のパルスから7SEGに数字を表示する */
void Pulse2Digits(int *pulse, byte num)
{
  for (int i = 0; i < num; i++) {
    SetDigit(Pulse2Digit(pulse[i]), i);
  }
}

void InitSeg(void)
{
  byte data[] = { 0xff, 0xff, 0xff, 0xff };
  display.setBrightness(0x0f);
  // All segments on
  display.setSegments(data);
}

/* パルス幅を数字に変換 */
byte Pulse2Digit(int pulse)
{
  byte digit = 0xff;
  for (int i = 0; i < sizeof(p2d) / sizeof(p2d[0]); i++) {
    if ( p2d[i].pmin <= pulse && pulse <= p2d[i].pmax) {
      digit = p2d[i].digit;
      break;
    }
  }
  return digit;
}

/* 指定した位置に指定した数字を表示 */
void SetDigit(byte num, byte pos)
{
  static byte data[] = {0x00, 0x00, 0x00, 0x00};

  if (pos < 4) {
    if (0 <= num && num <= 9) {
      data[pos] = display.encodeDigit(num);
    } else {
      data[pos] = 0;
    }
  } else {
    return -1;
  }

  display.setSegments(data);

  return 0;
}

