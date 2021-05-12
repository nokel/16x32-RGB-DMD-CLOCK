16x32 Pixel RGB Dot matrix clock that displays date, temperature, and news headlines

Code created by GulJanjua (https://www.fiverr.com/guljanjua) and anyone else mentioned within source-code files.

Originally this idea was created because I couldn't find anyone else who had made a clock with the adafruit 16x32 RGB matrix, and my previous clock annoyed me because I had to manually change the time due to drift of the clock module and DST, so I comissioned GulJanjua to help me make a new one that would do all of that, plus display current headlines considering it would have been a waste of a module if it were restricted to updating the clock when it drifted and when dst was around...

There are a few hiccups, but so far the clock can

  1. Display time
  2. Display date
  3. Update time from NTP server (although recently there's been a bug stopping this one from working 100%)
  4. Display headlines from 3 RSS sources

If you want the same setup as what I've currently got, it's comprised of 
  - Arduino Mega (https://www.jaycar.com.au/duinotech-mega-2560-r3-board-for-arduino/p/XC4420)
  - ESP32        (https://www.jaycar.com.au/esp32-main-board-with-wifi-and-bluetooth-communication/p/XC3800)
  - Adafruit 16x32 RGB DMD (https://www.adafruit.com/product/420)
  - Mega Prototype Shield (https://www.jaycar.com.au/mega-prototype-shield-with-breadboard-for-arduino/p/XC4416)
  - LM2596S stepdown converter (https://www.jaycar.com.au/arduino-compatible-dc-voltage-regulator/p/XC4514)
  - DS3231 RTC (https://www.ebay.com.au/itm/DS3231-AT24C32-IIC-I2C-Precision-RTC-Real-Time-Clock-Arduino-Module/273707923489?ssPageName=STRK:MEBIDX:IT&_trksid=p2060353.m2749.l2649) 
  - Analogue termperature sensor (https://www.jaycar.com.au/temperature-sensor-module-arduino-compatible/p/XC4494)*
  - 26Ah Gel battery (https://www.jaycar.com.au/12v-26ah-deep-cycle-sla-battery/p/SB1698) //battery is pretty overkill, It will last about 2-3 weeks without                                                                                                   additional power.
  - 40watt solar panel (https://www.jaycar.com.au/powertech-monocrystalline-solar-panel-40w/p/ZM9095)**
                 or    (https://www.jaycar.com.au/12v-40w-monocrystalline-solar-panel/p/ZM9056)
  - solar charge regulator (https://www.jaycar.com.au/12-24v-10a-solar-charge-controller-with-usb/p/MP3750) //put on setting 2, this will be correct for the                                                                                                                  26Ah gel battery preventing over and undercharging.

**The ZM9095 has been discontinued, but any 40 watt panel should provide more than enough to keep the battery from needing to be touched.

*The temperature can actually be measured with the DS3231, but apparently it can get warm with prolonged use, so I've put the temp sensor on a longish cable to avoid innacuracy.

HOW TO INSTALL:

Add these links to "Additional Boards Manager" under settings:
  - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  - http://arduino.esp8266.com/stable/package_esp8266com_index.json

Download "shoddyxml" and add to arduino library (https://github.com/garretlab/shoddyxml)

Select "board" from the "tools" dropdown menu

  - The .ino file titled "Arduino" should be uploaded to an Arduino Mega
  - The .ino file titled "ESP32" Should be selected as "ESP32 Dev Module"

(If you choose to use the files in the folder "ESP32" they should be uploaded to an ESP32 module, NOT an ESP8266, which was originally what it was going to use, the current unedited code will work with ESP32, sorry for the confusion.)
