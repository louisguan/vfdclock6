
void VFDmakeTime() {

  char out[13] = {0};
  vfd.displayOut[0] = '\0';
  sprintf(out, "%2d%02d%02d", hour(), minute(), second());  // create time string      '220828'
  sprintf(vfd.displayOut, "%10s", out);                     // left pad the string:    '    220228'
  strcat(vfd.displayOut, " ");                              // put a space on the end: '    220228 '

}

void VFDmakeDate() {

  char out[13] = {0};
  vfd.displayOut[0] = '\0';
  sprintf(out, "%02d%s%02d", day(), VFDmonth[month()], year()); // '14JAN2017'
  sprintf(vfd.displayOut, "%10s", out);                         // ' 14JAN2017'
  strcat(vfd.displayOut, " ");                                  // ' 14JAN2017 '
}

void VFDmakeDSTinfo() {

  char out[13] = {0};
  vfd.displayOut[0] = '\0';
  sprintf(out, "DST %s", dst.dset);                         // 'DST BST'
  sprintf(vfd.displayOut, "%10s", out);                     // '   DST BST'
  strcat(vfd.displayOut, " ");                              // '   DST BST '
}

// used on startup when setting the time
void VFDshowClockIcon() {

  char out[33] = {0};
  strcpy(out, vfd.blank);
  out[31 - 1] = '1';
  out[VFDlookup[VFDclockIcon]] = '1';
  unsigned long data = strtoul(out, NULL, 2);
  shiftOut_32(VFD_DATA, VFD_CLOCK, LSBFIRST, data);
}

// the binary output for the display is built once a second.
void VFDbuildOutput() {

  vfd.grids = 0;
  for (int e = 0; e < 12; e++) {
    vfd.out[e][0] = '\0';

    if (vfd.displayOut[e] == ' ') {
      strcpy(vfd.out[e], vfd.blank);
    }
    else {
      for (int ii = 0; ii < 36; ii++) {
        if (vfd.displayOut[e] == VFDdigitsLookup[ii]) {
          strcpy(vfd.out[e], vfd.lookup[ii]);
          break;
        }
      }
    }

    if ((vfd.grids == 7 || vfd.grids == 5) && aTimes.showDate == false) {
      vfd.out[e][VFDlookup[VFDcolon]] = '1';
    }

    if (vfd.grids == 1 && vfd.showClockIcon == true) {
      vfd.out[e][VFDlookup[VFDclockIcon]] = '1';
    }

    vfd.out[e][31 - vfd.grids] = '1';
    vfd.grids++;
  }


}

void VFDshowTime() {

  for (int e = 0; e < 12; e++) {

    unsigned long data = strtoul(vfd.out[e], NULL, 2);
    shiftOut_32(VFD_DATA, VFD_CLOCK, LSBFIRST, data);

    delayMicroseconds(350);
    regOne.allOff();
  }

}
