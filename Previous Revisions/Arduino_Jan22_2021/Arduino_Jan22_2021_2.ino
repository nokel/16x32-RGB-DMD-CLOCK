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
#include <SoftwareSerial.h>

SoftwareSerial mySerial(6, 7);

RTC_DS3231 rtc;


#define CLK  8
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);

#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

const char str[] PROGMEM = "Adafruit 16x32 RGB LED Matrix";
int16_t    textX         = matrix.width(),
           textMin       = sizeof(str) * -12,
           hue           = 0;

int8_t ball[3][4] = {
  {  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { 17, 15,  1, -1 },
  { 27,  4, -1,  1 }
};
static const uint16_t PROGMEM ballcolor[3] = {
  0x0080, // Green=1
  0x0002, // Blue=1
  0x1000  // Red=1
};

//int16_t hue = 0 , textX = matrix.width() , textMin = 0;
unsigned long upTimer;
//String str;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(2);
  delay(1000);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
   // Serial.flush();
  //  abort();
  }

  /*if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }*/
  mySerial.println("Update");
  /* String formattedDate;
    String dayStamp;
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    int year = dayStamp.substring(0 , 4).toInt();
    int month = dayStamp.substring(5 , 7).toInt();
    int day = dayStamp.substring(8 , 10).toInt();
    int hour = dayStamp.substring(11, 13).toInt();
    int min = dayStamp.substring(14 , 16).toInt();
    rtc.adjust(DateTime(year, month, day, hour, min , 0));*/
  Serial.println("Hello starting");
}

void loop() {
  DateTime now = rtc.now();
  int temperature = Thermister(analogRead(A3));
Serial.println(temperature);
 // uint8_t  x = now.dayOfTheWeek();
  //String datentime = getDayOfWeek(x) + " " + String(now.day(), DEC) + monthGet(now.month()) + String(now.year(), DEC);

//Serial.println(datentime);

  /* if (mySerial.available()) {
     str = mySerial.readStringUntil('\n');
     Serial.println(str);
     textMin = str.length() * -12;
    }

    if (millis() - upTimer > 43200000) {
     mySerial.println("Update");
     String formattedDate;
     while (1) {
       if (mySerial.available() > 0) {
         formattedDate = mySerial.readString();
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
    }*/

  byte i;

  // Clear background
 // matrix.fillScreen(0);

  // Bounce three balls around
  for (i = 0; i < 3; i++) {
    // Draw 'ball'
    matrix.fillCircle(ball[i][0], ball[i][1], 5, pgm_read_word(&ballcolor[i]));
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];
    // Bounce off edges
    if ((ball[i][0] == 0) || (ball[i][0] == (matrix.width() - 1)))
      ball[i][2] *= -1;
    if ((ball[i][1] == 0) || (ball[i][1] == (matrix.height() - 1)))
      ball[i][3] *= -1;
  }

  matrix.setTextColor(matrix.ColorHSV(hue, 255, 255, true));
  matrix.setCursor(textX, 1);
  matrix.print(F2(str));

  // Move text left (w/wrap), increase hue
  if ((--textX) < textMin) textX = matrix.width();
  hue += 7;
  if (hue >= 1536) hue -= 1536;

#if !defined(__AVR__)
  // On non-AVR boards, delay slightly so screen updates aren't too quick.
  delay(20);
#endif

  // Update display
  matrix.swapBuffers(false);

}

double Thermister(int RawADC) {
  double Temp;
  Temp = log(((10240000 / RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15; // Convert Kelvin to Celcius
  return Temp;
}

String monthGet(int x) {
  switch (x) {
    case 1:
      return "Jan";
      break;
    case 2:
      return "Feb";
      break;
    case 3:
      return "Mar";
      break;
    case 4:
      return "Apr";
      break;
    case 5:
      return "May";
      break;
    case 6:
      return "Jun";
      break;
    case 7:
      return "Jul";
      break;
    case 8:
      return "Aug";
      break;
    case 9:
      return "Sep";
      break;
    case 10:
      return "Oct";
      break;
    case 11:
      return "Nov";
      break;
    case 12:
      return "Dec";
      break;
  }
}

String getDayOfWeek(uint8_t  x) {
  switch (x) {
    case 0: return "Sun"; break;
    case 1: return "Mon"; break;
    case 2: return "Tue"; break;
    case 3: return "Wed"; break;
    case 4: return "Thu"; break;
    case 5: return "Fri"; break;
    case 6: return "Sat"; break;
  }
}
