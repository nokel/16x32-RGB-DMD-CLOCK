#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "shoddyxml.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

const char * WIFI_SSID = "SSID";
const char * WIFI_PASS = "PASS";

const struct site_t {
  const char *title;
  const char *url;
  const char *contentsToDisplay;
} sites[] = {
  {"ABC NEWS", "https://www.abc.net.au/news/feed/51120/rss.xml", "title"},
  {"SBS News", "https://www.sbs.com.au/news/feed", "title"},
  {"Aljazeera English", "https://www.aljazeera.com/xml/rss/all.xml", "title"},
};

const int delayPerRSS = 10000;

int itemDepth = 0;
int lastTagMatches = 0;
char *contentsToDisplay;

int httpGetChar();

WiFiMulti wifiMulti;
HTTPClient http;
WiFiClient *stream;
shoddyxml x(httpGetChar);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void foundXMLDeclOrEnd() {

}

void foundPI(char *s) {

}

void foundSTag(char *s, int numAttributes, attribute_t attributes[]) {
  if (strcmp(s, "item") == 0) {
    itemDepth++;
  }

  if (strcmp(s, contentsToDisplay) == 0) {
    lastTagMatches = 1;
  } else {
    lastTagMatches = 0;
  }
}

void foundETag(char *s) {
  if ((itemDepth == 1) && (strcmp(s, contentsToDisplay) == 0)) {
    if (Serial2.available()) {
      String s = Serial2.readString();
      Serial.println(s);
      const char *cstr = s.c_str();
      if (strstr(cstr, "Update") != NULL) {
        while (!timeClient.update()) {
          timeClient.forceUpdate();
        }
        String formattedDate;
        String dayStamp;

        formattedDate = timeClient.getFormattedDate();
        Serial2.println("TIMEUPDATE:" + formattedDate);
        Serial.println("TIMEUPDATE:" + formattedDate);
      }
    }
    else {
      Serial.println("");
      Serial2.println("");
      delay(20000);
    }
  }
  if (strcmp(s, "item") == 0) {
    itemDepth--;
  }
}

void foundEmptyElemTag(char *s, int numAttributes, attribute_t attributes[]) {

}

void foundCharacter(char c) {
  if ((itemDepth == 1) && (lastTagMatches == 1)) {
    Serial.print(c);
    Serial2.print(c);
  }
}

void foundElement(char *s) {

}

int httpGetChar() {
  if (http.connected()) {
    if (stream->available()) {
      return stream->read();
    } else {
      return 0;
    }
  }
  return EOF;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  wifiMulti.addAP(ssid, password);
  x.foundXMLDecl = foundXMLDeclOrEnd;
  x.foundXMLEnd = foundXMLDeclOrEnd;
  x.foundPI = foundPI;
  x.foundSTag = foundSTag;
  x.foundETag = foundETag;
  x.foundEmptyElemTag = foundEmptyElemTag;
  x.foundCharacter = foundCharacter;
  x.foundElement = foundElement;
}

void loop() {
  for (int i = 0; i < sizeof(sites) / sizeof(struct site_t); i++) {
    if ((wifiMulti.run() == WL_CONNECTED)) {
      itemDepth = 0;
      lastTagMatches = 0;
      //Serial.println(sites[i].title);
      contentsToDisplay = const_cast<char*>(sites[i].contentsToDisplay);
      http.begin(sites[i].url);
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          stream = http.getStreamPtr();
          x.parse();
        }
      }
      http.end();
      delay(delayPerRSS);
    } else {
      wifiMulti.addAP(ssid, password);
    }
  }
}
