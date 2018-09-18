#include <Arduino.h>                    // used in PlatformIO

#include <stdio.h>
#include <Adafruit_NeoPixel.h>          //
#ifdef __AVR__
#include <avr/power.h>
#endif

#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <ShiftOutX.h>                 // http://playground.arduino.cc/Main/ShiftOutX  v1.0
#include <ShiftPinNo.h>

#define NEO_PIN D3
#define NEO_LEDS 12
#define MONITOR_LED LED_BUILTIN
#define VFD_CLOCK D7
#define VFD_LATCH D6
#define VFD_DATA D8
#define VFD_BLANK D5
#define MCP_INT D4

#define ROT_A 3
#define ROT_B 2
#define ROT_BTN 4
#define ROT_RED 5
#define ROT_GRN 6
#define ROT_BLU 7

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_LEDS, NEO_PIN, NEO_GRB + NEO_KHZ800);

shiftOutX regOne(VFD_LATCH, VFD_DATA, VFD_CLOCK, MSBFIRST, 4);

#include <TimeLib.h>

#include <Wire.h>
#include "Adafruit_MCP23008.h"        // https://github.com/adafruit/Adafruit-MCP23008-library
Adafruit_MCP23008 mcp0;
// WiFi stuff
#include <ESP8266WiFi.h>              // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <WiFiUdp.h>

char ssid[] = "YOUR_WIFI_SSID";       // your network SSID (name)
char pass[] = "YOUR_WIFI_PASSWORD";   // your network password
unsigned int localPort = 2390;        // local port to listen for UDP packets
IPAddress timeServerIP;               // time.nist.gov NTP server address
const char* ntpServerName = "uk.pool.ntp.org";
const int NTP_PACKET_SIZE = 48;       // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];   // buffer to hold incoming and outgoing packets
WiFiUDP udp;

boolean isrMonitorON = true;
byte monitorState = LOW;
char cDigit[6];
unsigned long ntpUnixTime;
boolean runonce = false;
unsigned long nowMillis = 0;
int rotaryCurrent = 0;
int rotaryNew = 0;

#include "vfd_digits5.h"
ATIMERS aTimes;
VFD vfd;
DST dst;

#include "vfd_display.h"
#include "vfd_dst.h"
#include "vfd_ntp.h"

boolean colours[8][3] = { {1, 1, 1},  // off (black)
                          {1, 1, 0},  // blue
                          {1, 0, 1},  // green
                          {1, 0, 0},
                          {0, 1, 1},  // red
                          {0, 1, 0},
                          {0, 0, 1},
                          {0, 0, 0}}; // white
const int leds[3] = { ROT_RED, ROT_GRN, ROT_BLU };

void setColours(boolean colour[]) {
    for (int i=0; i<3; i++) {
       mcp0.digitalWrite(leds[i], colour[i]);
    }
    return;
}

void isrMonitor() {
  if (isrMonitorON == false) {
    return;
  }

  Serial.println("int");

}

int readRotarySwitch(int current, int minimum, int maximum) {

  int r = 0;

  byte rotA = mcp0.digitalRead(ROT_A);
  byte rotB = mcp0.digitalRead(ROT_B);

  if ((!rotA) && (aTimes.rotAprev)) {
    if (rotB) {
      r++; // clockwise
    }
    else {
      r--; // counter-clockwise
    }
  }
  aTimes.rotAprev = rotA;
  r = current + r;
  if (r < minimum) {
    r = maximum;
  }
  if (r > maximum) {
    r = minimum;
  }

  while (mcp0.digitalRead(ROT_A) != rotA) {
    delay(1);    // wait until button release;
  }

  return r;
}

boolean readRotaryButton() {

  boolean b = mcp0.digitalRead(ROT_BTN);
  if (b == true ) {
    while (mcp0.digitalRead(ROT_BTN) != b) {
      delay(1);    // wait until button release;
    }
    return true;
  }
  return false;
}

void setup() {

  delay(1000);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial.println("setup");

//  pinMode(MONITOR_LED, OUTPUT);
//  digitalWrite(MONITOR_LED, LOW);

  // setup the rotary encoder
  mcp0.begin(0);      // use default address 0
  for (byte n = 2; n <= 4; n++) {
    mcp0.pinMode(n, INPUT);
    mcp0.pullUp(n, HIGH);
  }
  mcp0.pullUp(ROT_BTN, LOW);

  for (byte n = 5; n <= 7; n++) {
    mcp0.pinMode(n, OUTPUT);
    mcp0.digitalWrite(n, HIGH);
  }
  delay(10);
  setColours(colours[4]);    // red

  // setup the VFD variables
  vfd.blank[0] = '\0';
  for (int i = 0; i < 32; i++) {
    strcat(vfd.blank, "0");
  }

  // build a predefined lookup table 0-9 and A-Z
  for (int i = 0; i < 36; i++) {
    strcpy(vfd.lookup[i], vfd.blank);        // start with an array of zeros

    for (int element = 0; element < 17; element++) {
      if (VFDdigits[i][element] == '1') {
        vfd.lookup[i][VFDlookup[VFDdigitPos[element]]] = '1'; // find the pin on the display for the chosen element
      }
    }
    yield();
  }

  // setup the shift register
  pinMode(VFD_CLOCK, OUTPUT);
  pinMode(VFD_LATCH, OUTPUT);
  pinMode(VFD_DATA, OUTPUT);
  pinMode(VFD_BLANK, OUTPUT);
  analogWrite(VFD_BLANK, 255);

  pinMode(MCP_INT,INPUT);
//  mcp0.setupInterrupts(true,LOW);
//  mcp0.setupInterruptPin(mcpPinA,FALLING);
//  attachInterrupt(digitalPinToInterrupt(MCP_INT), isrMonitor, RISING);

  // start the neopixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  regOne.allOff();
  VFDshowClockIcon();
  delay(1000);

  // connect to a WiFi network
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  setColours(colours[2]);    // green
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  udp.begin(localPort);
  delay(1000);

  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  do {
    delay(1000);
    ntpUnixTime = getNTPtime();
  } while (ntpUnixTime == 0);

  setTime(ntpUnixTime);       // Time.h
  setDaylightSavingsDates();
  setCurrentDST(now());       // set GMT or BST and the offset
  dst.currentYear = year();
  setTime(now() + dst.offset); // Time.h

  Serial.print("unixtime: ");
  Serial.println(now());
  Serial.print("date: ");
  printDateTime(now());
  Serial.print(" ");
  Serial.println(dst.currentDST);

  regOne.allOff();
  setColours(colours[0]);    // off
  Serial.println("showtime");

}

void loop() {

  aTimes.currentMillis = millis();

  if (dst.currentYear != year()) {
    setDaylightSavingsDates();
    setCurrentDST(now());
    dst.currentYear = year();
  }

  if ((now() == dst.BST || now() == dst.GMT) && dst.dset == false) {
    vfd.showClockIcon = true;
    setCurrentDST(now());
    dst.dset = true;
    setTime(now() + dst.offset);
  }

  if (second() % 2 == 0) {
    digitalWrite(MONITOR_LED, HIGH);
  }
  else {
    digitalWrite(MONITOR_LED, LOW);
    vfd.showClockIcon = false;
    dst.dset = false;
  }

//    if (aTimes.currentMillis >= (aTimes.rotaryTime + aTimes.rotaryPause)) {
//      rotaryNew = readRotarySwitch(rotaryCurrent, 1, 10);
//      aTimes.rotaryTime = aTimes.currentMillis;
//    }
//    if (rotaryNew != rotaryCurrent) {
//      if (rotaryNew < rotaryCurrent) {
//        aTimes.rotAprev = -1;
//      }
//      else {
//        aTimes.rotAprev = 1;
//      }
//
//      Serial.print("rotary ");
//      Serial.print(rotaryNew);
//      Serial.print("\t");
//      Serial.println(aTimes.rotAprev);
//
//      rotaryCurrent = rotaryNew;
//    }

  boolean button = readRotaryButton();
  if (button == true) {
    setColours(colours[5]);    //
    aTimes.dateDisplaySec = 0;
    aTimes.showDate = true;
    aTimes.previousSecond++;   // dont wait unil the second has elapsed
  }

  // build the string for the display, something like: '    220228 '
  if (second() != aTimes.previousSecond) {
    if (aTimes.showDate == true) {
      VFDmakeDate();
      aTimes.dateDisplaySec++;
      if (aTimes.dateDisplaySec > 8) {
        aTimes.showDate = false;
        setColours(colours[0]);    // off
        VFDmakeTime();
      }
    }
    else {
      VFDmakeTime();  // put the formatted time into vfd.displayTime: 220228
    }
    VFDbuildOutput();
    aTimes.previousSecond = second();
  }

  if (aTimes.ntpTimeSet == true) {
    WiFi.hostByName(ntpServerName, timeServerIP);
    sendNTPpacket(timeServerIP);          // send an NTP packet to a time server
    if (millis() >= (nowMillis + 800)) {  // wait to see if a reply is available
      ntpUnixTime = getNTPtime();
      if (ntpUnixTime != 0) {
        setTime(ntpUnixTime + dst.offset); // Time.h
        Serial.print("ntp timeset: ");
        printDateTime(now());
        Serial.print(" ");
        Serial.println(dst.currentDST);
      }
    }
    aTimes.ntpTimeSet = false;
    runonce = true;
  }

  if (hour() % 2 == 0 && minute() == 5 && second() == 8 && runonce == false) {
    vfd.showClockIcon = true;
    aTimes.ntpTimeSet = true;
    nowMillis = millis();
  }
  if (hour() % 2 == 0 && minute() == 5 && second() == 9 && runonce == true) {
    runonce = false;
  }

  VFDshowTime();
  yield();
}
