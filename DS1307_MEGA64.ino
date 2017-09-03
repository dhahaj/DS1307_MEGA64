#include <Wire.h>
#include "RTClib.h"
#include <TaskScheduler.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x3F);

void t1Callback();
void serialEvent();
Task t1(3000, TASK_FOREVER, &t1Callback);
Task t2(100, TASK_FOREVER, &serialEvent);
Scheduler runner;

DS1307 rtc;

void t1Callback() {
  DateTime now = rtc.now();

  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.setCursor(0, 1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  //  lcd.println();

  //  Serial.print(" since midnight 1/1/1970 = ");
  //  Serial.print(now.unixtime());
  //  Serial.print("s = ");
  //  Serial.print(now.unixtime() / 86400L);
  //  Serial.println("d");
}

void setup () {
  Serial.begin(57600);
  lcd.begin(16, 2);
  Wire.begin();
  rtc.begin();

  //  if (! rtc.isrunning()) {
  //    Serial.println("RTC is NOT running!");
  //    // following line sets the RTC to the date & time this sketch was compiled
  //    rtc.adjust(DateTime(__DATE__, __TIME__));
  //  }
  runner.init();
  runner.addTask(t1);
  t1.enable();
  runner.addTask(t2);
  t2.enable();
}

void loop () {
  //serialEvent();
  runner.execute();
}

void serialEvent() {
  static String s = "";
  if (Serial.available() > 0) {
    char c;
    if ((c = (char)Serial.read()) != '\n') {
      s += c;
    } else {
      Serial.println("Recieved: " + s + "\n");
      if (s.indexOf('-') != -1) {
        DateTime dt = rtc.now();
        TimeDelta td = TimeDelta(s.toInt());
        rtc.adjust(dt + td);
      } else {
        s.replace("-", "");
        DateTime dt = rtc.now();
        TimeDelta td = TimeDelta(s.toInt());
        rtc.adjust(dt - td);
      }
      s = "";
    }
  }
}

