
// I think this NTP section was adapted from: https://www.arduino.cc/en/Tutorial/UdpNTPClient
unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

unsigned long getNTPtime() {

  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
    return 0;
  }

  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  const unsigned long seventyYears = 2208988800UL;   // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  unsigned long epoch = secsSince1900 - seventyYears;  // subtract seventy years:
  return epoch;
}

void printUnixTime (unsigned long unixtime) {

  // print the hour, minute and second:
  Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
  Serial.print((unixtime  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  Serial.print(':');
  if (((unixtime % 3600) / 60) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.print((unixtime  % 3600) / 60); // print the minute (3600 equals secs per minute)
  Serial.print(':');
  if ((unixtime % 60) < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.println(unixtime % 60); // print the second
}

// I don't think this Daylight Savings section works.
void printDateTime(time_t t) {
  char tdate[25] = {0};
  sprintf(tdate,"%02d %s %d, %02d:%02d:%02d",day(t), txtMonth[month(t)], year(t), hour(t), minute(t), second(t));
  Serial.print(tdate);
}

void setCurrentDST(time_t n) {

  if ((int)n > (int)dst.BST && (int)n < (int)dst.GMT) {
    strcpy(dst.currentDST,"BST");
    dst.offset = 60*60;
  }
  else {
    strcpy(dst.currentDST,"GMT");
    dst.offset = 0;
  }
}

time_t findTheLastSunday(time_t y) {

    tmElements_t tm;
    int nowMonth = (int)month(y);
    time_t lastSunday = 0;

    int d = 32;
    tm.Hour = hour(y);
    tm.Minute = minute(y);
    tm.Second = second(y);
    tm.Day = d;
    tm.Month = month(y);
    tm.Year = year(y)-1970;

    while (d > 0) {

      tm.Day = d;
      time_t t = makeTime(tm);

      d--;
      if (month(t) != nowMonth) {
        continue;
      }

      if (weekday(t) == 1) {
        lastSunday = abs(t);
        break;
      }
    }
    return lastSunday;
}

void setDaylightSavingsDates() {

  tmElements_t tm;
  tm.Hour = 2;
  tm.Minute = 0;
  tm.Second = 0;
  tm.Day = 20;
  tm.Month = 3;  // march
  tm.Year = year()-1970;
  time_t tBST = makeTime(tm);
  dst.BST = findTheLastSunday(tBST);

  tm.Month = 10;  // october
  time_t tGMT = makeTime(tm);
  dst.GMT = findTheLastSunday(tGMT);
}
