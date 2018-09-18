

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
