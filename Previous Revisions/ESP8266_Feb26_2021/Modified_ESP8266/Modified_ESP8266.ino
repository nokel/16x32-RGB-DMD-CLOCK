/*
  Author: GulJanjua
  Date  : February 26, 2021

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

#include <WiFi.h>
#include "RSSReader.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// Array of feed URLs
const char *rssFeedURLs [] = {
  "https://www.abc.net.au/news/feed/51120/rss.xml",
  "https://www.aljazeera.com/xml/rss/all.xml",
  "https://www.sbs.com.au/news/feed"
};

#define NUMBER_OF_FEEDS (sizeof(rssFeedURLs) / sizeof(char *))
int rssFeedIndex;

// Set your WiFi login credentials
const char * WIFI_SSID = "SSID";
const char * WIFI_PASS = "PASS";

RSSReader reader = RSSReader(3000);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

unsigned long upTimer;
char buffer[TITLE_BUFFER_SIZE];

void titleCallback(char *titleStr) {
  Serial.println(titleStr);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Setup callbacks for title and pubDate tags in RSS XML
  reader.setTitleCallback(&titleCallback);
  rssFeedIndex = 0;

  timeClient.begin();
  timeClient.setTimeOffset(36000);
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String formattedDate;
  String dayStamp;

  formattedDate = timeClient.getFormattedDate();
}

void loop() {
  delay(1);
  const char *url = rssFeedURLs[rssFeedIndex];
  bool result = reader.read(url);
  rssFeedIndex++;
  rssFeedIndex %= NUMBER_OF_FEEDS;

  if (Serial.available()) {
    String s = Serial.readString();
    const char *cstr = s.c_str();
    if (strstr(cstr, "Update") != NULL) {
      while (!timeClient.update()) {
        timeClient.forceUpdate();
      }
      String formattedDate;
      String dayStamp;

      formattedDate = timeClient.getFormattedDate();
      Serial.println(formattedDate);
    }
  }
}
