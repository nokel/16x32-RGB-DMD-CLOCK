/*
  Author: GulJanjua
  Date  : January 22, 2021

  Software Requirements:
  ----------------------
  #include <ESP8266WiFi.h>
  #include "RSSReader.h"
  #include <NTPClient.h>
  #include <WiFiUdp.h>

  Hardware Requirements:
  ----------------------
  -Arduino Uno Wifi
  -DS3231
  -Temperature sensor

  Project Requirents:
  -------------------
  • Auto update
*/

#include <math.h>
#include "RTClib.h"
#include <RGBmatrixPanel.h>

RTC_DS3231 rtc;


#define CLK  8
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);

int16_t hue = 0 , textX = matrix.width() , textMin = 0;
unsigned long upTimer;
String str;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(2);
  delay(1000);
  Serial.println("Update");
  String formattedDate;
  String dayStamp;
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  int year = dayStamp.substring(0 , 4).toInt();
  int month = dayStamp.substring(5 , 7).toInt();
  int day = dayStamp.substring(8 , 10).toInt();
  int hour = dayStamp.substring(11, 13).toInt();
  int min = dayStamp.substring(14 , 16).toInt();
  rtc.adjust(DateTime(year, month, day, hour, min , 0));
}

void loop() {
  delay(1);
  byte i;
  matrix.fillScreen(0);

  int temperature = Thermister(analogRead(A3));

  if (Serial.available()) {
    str = Serial.readString();
    textMin = str.length() * -12;
  }

  if (millis() - upTimer > 43200000) {
    Serial.println("Update");
    String formattedDate;
    while (1) {
      if (Serial.available() > 0) {
        formattedDate = Serial.readString();
        break;
      }
    }
    String dayStamp;
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    int year = dayStamp.substring(0 , 4).toInt();
    int month = dayStamp.substring(5 , 7).toInt();
    int day = dayStamp.substring(8 , 10).toInt();
    int hour = dayStamp.substring(11, 13).toInt();
    int min = dayStamp.substring(14 , 16).toInt();
    rtc.adjust(DateTime(year, month, day, hour, min , 0));
    upTimer = millis();
  }

  matrix.setTextColor(matrix.ColorHSV(hue, 255, 255, true));
  matrix.setCursor(textX, 1);
  matrix.print(str);
  if ((--textX) < textMin) textX = matrix.width();
  hue += 7;
  if (hue >= 1536) hue -= 1536;
  matrix.swapBuffers(false);

}

double Thermister(int RawADC) {
  double Temp;
  Temp = log(((10240000 / RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15; // Convert Kelvin to Celcius
  return Temp;
}
