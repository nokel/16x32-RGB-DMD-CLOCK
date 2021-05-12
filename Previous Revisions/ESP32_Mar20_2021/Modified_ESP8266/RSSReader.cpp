/*
 *  RSSReader.cpp - RSSReader Class Functions
 *
 *  Concept, Design and Implementation by: Craig A. Lindley
 *  Last Update: 11/06/2015
 */

#include <WiFi.h>
#include "RSSReader.h"

#define HTTPPORT 80

// Connection for DST pushbutton
#define DST_SW 5

// States of the Finite State Machine (FSM) used to parse tags out of RSS XML
enum STATES {INIT, CHK_START, TEST_FIRST_CHAR, CHK_DATE, CHK_TITLE, CHK_DESC, GET_DATE, GET_TITLE, GET_DESC};
enum STATES state;

const char *date = "pubDate>";
const char *title = "title>";
const char *desc  = "description>";

// Finite state machine for parsing XML tags of interest
void RSSReader::runStateMachine(char ch) {

  switch (state) {
    case INIT:
      // Make buffers empty
      memset(titleBuffer, 0, sizeof(titleBuffer));
      memset(descBuffer, 0, sizeof(descBuffer));
      memset(dateBuffer, 0, sizeof(dateBuffer));

      // Next state
      state = CHK_START;
      break;

    case CHK_START:
      if (ch == '<') {
        state = TEST_FIRST_CHAR;
      }
      break;

    case TEST_FIRST_CHAR:
      if (ch == date[0]) {
        dateIndex = 1;
        state = CHK_DATE;
      }
      else if (ch == title[0]) {
        titleIndex = 1;
        state = CHK_TITLE;
      } else if (ch == desc[0]) {
        descIndex = 1;
        state = CHK_DESC;
      } else  {
        state = CHK_START;
      }
      break;

    case CHK_DATE:
      if (ch != date[dateIndex]) {
        state = CHK_START;
        break;
      }
      dateIndex++;
      if (ch == '>') {
        dateIndex = 0;
        state = GET_DATE;
      }
      break;

    case CHK_TITLE:
      if (ch != title[titleIndex]) {
        state = CHK_START;
        break;
      }
      titleIndex++;
      if (ch == '>') {
        titleIndex = 0;
        state = GET_TITLE;
      }
      break;

    case CHK_DESC:
      if (ch != desc[descIndex]) {
        state = CHK_START;
        break;
      }
      descIndex++;
      if (ch == '>') {
        descIndex = 0;
        state = GET_DESC;
      }
      break;

    case GET_TITLE:
      if (ch != '<') {
        titleBuffer[titleIndex++] = ch;
      } else  {
        titleBuffer[titleIndex++] = '\0';
        if ((strlen(titleBuffer) != 0) && (titleCallback != NULL)) {
          titleCallback(titleBuffer);
        }
        state = CHK_START;
      }
      break;

    case GET_DESC:
      if ((ch != '<') && (ch != '&')) {
        descBuffer[descIndex++] = ch;
      } else  {
        descBuffer[descIndex++] = '\0';
        if ((strlen(descBuffer) != 0) && (descCallback != NULL)) {
          descCallback(descBuffer);
        }
        state = CHK_START;
      }
      break;

    case GET_DATE:
      if (ch != '<') {
        dateBuffer[dateIndex++] = ch;
      } else  {
        // Just return the date not the time
        dateBuffer[17] = '\0';
        if ((strlen(dateBuffer) != 0) && (dateCallback != NULL)) {
          dateCallback(dateBuffer);
        }
        state = CHK_START;
      }
      break;
  }
}

// Class Constructor
RSSReader::RSSReader(int _timeoutInMS) {
  // Save incoming timeout value
  timeoutInMS = _timeoutInMS;

  // Setup DST pushbutton switch
  pinMode(DST_SW, INPUT_PULLUP);
}

// Assign the RSSReader's pubDate callback function
void RSSReader::setPubDateCallback(pt2Function _dateCallback) {
  dateCallback = _dateCallback;
}

// Assign the RSSReader's title callback function
void RSSReader::setTitleCallback(pt2Function _titleCallback) {
  titleCallback = _titleCallback;
}

// Assign the RSSReader's description callback function
void RSSReader::setDescCallback(pt2Function _descCallback) {
  descCallback = _descCallback;
}

// Parse URL and return result
// If successful, host and path storage areas have
// components of the URL.
bool RSSReader::parseURL(const char* url) {

  int index;
  const char *ptr;

  // Clear string storage
  memset(host, 0, sizeof(host));
  memset(path, 0, sizeof(path));

  // Is there a protocol specified ?
  // If so skip it
  if ((ptr = strchr(url, ':')) != NULL)  {
    // Yes there was a protocol
    ptr += 3;
  } else  {
    // No protocol specified
    ptr = url;
  }

  // Search for host separator
  char *endPtr = strchr(ptr, '/');
  if (endPtr == NULL) {
    return false;
  }
  // Copy host string to host storage
  index = 0;
  while (ptr != endPtr) {
    host[index++] = *ptr++;
  }
  // The remainder of the string is the path
  strcpy(path, endPtr);

  // All went well
  return true;
}

// Make the connection to the RSS source and parse the returned
// results calling the callback function when a title or description
// element is found in the XML.
// Returns true if pushbutton switch was pressed to terminate feed; false otherwise.
bool RSSReader::read(const char *url) {

  WiFiClient client;

  // Parse the URL passed in
  if (! parseURL(url)) {
    return false;
  }

  // Initial state machine state
  state = INIT;

  // Use WiFiClient class to create TCP connections
  Serial.println("Connecting to host");
  while (! client.connect(host, HTTPPORT)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Host connection succeeded")+ Serial.println(WiFi.localIP());; // When connected to host; display Local IP

  // This will send the request to the server
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive" + "\r\n\r\n");

  // Read all the chars run them through state machine
  unsigned long timeoutMS = timeoutInMS + millis();
  while (timeoutMS > millis()) {
    int count = client.available();
    while (count--) {
      char ch = client.read();
      //      Serial.print(ch);
      runStateMachine(ch);
      timeoutMS = timeoutInMS + millis();

      // Check pushbutton
      delay(5);
      yield();
    }
    delay(250);
  }
  client.stop();
  Serial.println("\n\nDone");
  return false;
}
