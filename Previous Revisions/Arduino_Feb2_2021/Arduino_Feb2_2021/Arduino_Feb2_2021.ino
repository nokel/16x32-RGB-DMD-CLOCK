#include "RTClib.h"
#include <RGBmatrixPanel.h>

RTC_DS3231 rtc;

String daysOfTheWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

String mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Most of the signal pins are configurable, but the CLK pin has some
// special constraints.  On 8-bit AVR boards it must be on PORTB...
// Pin 11 works on the Arduino Mega.  On 32-bit SAMD boards it must be
// on the same PORT as the RGB data pins (D2-D7)...
// Pin 8 works on the Adafruit Metro M0 or Arduino Zero,
// Pin A4 works on the Adafruit Metro M4 (if using the Adafruit RGB
// Matrix Shield, cut trace between CLK pads and run a wire to A4).

//#define CLK  8   // USE THIS ON ADAFRUIT METRO M0, etc.
//#define CLK A4 // USE THIS ON METRO M4 (not M0)
#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

String str = "Adafruit 16x32 RGB LED Matrix";
int16_t    textX         = matrix.width(),
           textMin       = str.length() * -12,
           hue           = 0;
String str1 , str2;
/* int8_t ball[3][4] = {
  {  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { 17, 15,  1, -1 },
  { 27,  4, -1,  1 }
};
static const uint16_t PROGMEM ballcolor[3] = {
  0x0080, // Green=1
  0x0002, // Blue=1
  0x1000  // Red=1
}; */ 

unsigned long timer;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(1);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  DateTime now = rtc.now();
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  str1 = String(now.hour()) + ":" + String(now.minute());
  str =  String(now.day()) + mon[now.month()] + String(now.year());
  textMin       = str.length() * -12;
  textX         = matrix.width();
}

void loop() {
  DateTime now = rtc.now();
  if (millis() - timer > 30000) {
    int temperature = Thermister(analogRead(A3));
    str1 = String(now.hour()) + ":" + String(now.minute());
    str = daysOfTheWeek[now.dayOfTheWeek()] + String(now.day()) + mon[now.month()] + String(now.year()) + " (" + String(temperature) + "C) "+str2;
    textMin       = str.length() * -12;
    textX         = matrix.width();
    timer = millis();
    Serial.println(str1 + str);
  }
  byte i;
  if (Serial1.available()) {
    str2 = Serial1.readString();
  }
  // Clear background
  matrix.fillScreen(0);
  matrix.setTextColor(matrix.ColorHSV(155, 255, 100, true));
  matrix.setCursor(1, 0);
  matrix.print(str1);
  matrix.setCursor(textX, 8);
  matrix.print((str));

  // Move text left (w/wrap), increase hue
  if ((--textX) < textMin) textX =  matrix.width();
  hue += 7;
  Serial.println(textX);
  if (hue >= 1536) hue -= 1536;
  matrix.swapBuffers(false);
  delay(20);
}

double Thermister(int RawADC) {
  double Temp;
  Temp = log(((10240000 / RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15; // Convert Kelvin to Celcius
  return Temp;
}
