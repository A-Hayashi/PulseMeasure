#include <Arduino.h>
/* ノイズ除去のための3回同じパルス幅が入ったら表示するようにすること */

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
  //  delay(1000);

  for (int i = 0; i < sizeof(meas) / sizeof(meas[0]); i++) {
    width[i] = meas[i].GetWidth();
    Serial.print(pulse[i]);
    Serial.print("/");
    Serial.print(width[i]);
    Serial.print("\t");
  }
  Serial.println("");

  for (int i = 0; i < sizeof(servo) / sizeof(servo[0]); i++)  {
    pulse[i] = incPulse(pulse[i], 100);
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
  {0, 595, 0},
  {596, 698, 1},
  {699, 801, 2},
  {802, 904, 3},
  {905, 1008, 4},
  {1009, 1111, 5},
  {1112, 1214, 6},
  {1215, 1317, 7},
  {1318, 1420, 8},
  {1421, 1523, 9},
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

/* Arduino Servo Library  */
/* 角度とパルスの関係     */
void test(void) {
#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo 
  for (int i = 0; i <= 180; i++) {
    int value = map(i, 0, 180, MIN_PULSE_WIDTH,  MAX_PULSE_WIDTH);
    Serial.print(i);
    Serial.print(":");
    Serial.print(value);
    Serial.println("");
  }
}

//0:544
//1:554
//2:564
//3:574
//4:585
//5:595
//6:605
//7:616
//8:626
//9:636
//10:647
//11:657
//12:667
//13:678
//14:688
//15:698
//16:708
//17:719
//18:729
//19:739
//20:750
//21:760
//22:770
//23:781
//24:791
//25:801
//26:812
//27:822
//28:832
//29:843
//30:853
//31:863
//32:873
//33:884
//34:894
//35:904
//36:915
//37:925
//38:935
//39:946
//40:956
//41:966
//42:977
//43:987
//44:997
//45:1008
//46:1018
//47:1028
//48:1038
//49:1049
//50:1059
//51:1069
//52:1080
//53:1090
//54:1100
//55:1111
//56:1121
//57:1131
//58:1142
//59:1152
//60:1162
//61:1172
//62:1183
//63:1193
//64:1203
//65:1214
//66:1224
//67:1234
//68:1245
//69:1255
//70:1265
//71:1276
//72:1286
//73:1296
//74:1307
//75:1317
//76:1327
//77:1337
//78:1348
//79:1358
//80:1368
//81:1379
//82:1389
//83:1399
//84:1410
//85:1420
//86:1430
//87:1441
//88:1451
//89:1461
//90:1472
//91:1482
//92:1492
//93:1502
//94:1513
//95:1523
//96:1533
//97:1544
//98:1554
//99:1564
//100:1575
//101:1585
//102:1595
//103:1606
//104:1616
//105:1626
//106:1636
//107:1647
//108:1657
//109:1667
//110:1678
//111:1688
//112:1698
//113:1709
//114:1719
//115:1729
//116:1740
//117:1750
//118:1760
//119:1771
//120:1781
//121:1791
//122:1801
//123:1812
//124:1822
//125:1832
//126:1843
//127:1853
//128:1863
//129:1874
//130:1884
//131:1894
//132:1905
//133:1915
//134:1925
//135:1936
//136:1946
//137:1956
//138:1966
//139:1977
//140:1987
//141:1997
//142:2008
//143:2018
//144:2028
//145:2039
//146:2049
//147:2059
//148:2070
//149:2080
//150:2090
//151:2100
//152:2111
//153:2121
//154:2131
//155:2142
//156:2152
//157:2162
//158:2173
//159:2183
//160:2193
//161:2204
//162:2214
//163:2224
//164:2235
//165:2245
//166:2255
//167:2265
//168:2276
//169:2286
//170:2296
//171:2307
//172:2317
//173:2327
//174:2338
//175:2348
//176:2358
//177:2369
//178:2379
//179:2389
//180:2400

