
// pin mapping for the MAX6502 to VFD display
//  |driver 3               |driver 2                           |driver 1
// X| 1  0 11 10  9  8  7  6| 5  4  3  2  1  0 11 10  9  8  7  6| 5  4  3  2  1  0 11 10  9  8  7  6
//  | 1  2  3  4  5  6  7  8  9 10 11 -- grids
//                         elments -- 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
static const int VFDlookup[33] = {
   0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
};

static const int VFDgridPos[11] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11 };  // pins on the display
static const int VFDdigitPos[17] = {15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};  // pins on the display

static const char VFDdigitsLookup[36] {
  '0','1','2','3','4','5','6','7','8','9',
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};
//  0      1      2      3      4      5      6      7      8      9      10     11     12
static const char VFDmonth[13][4] = {
   "XXX", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

static const char txtMonth[13][4] = {
   "xxx", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const int VFDcolon = 14;      // grids 8 and
static const int VFDclockIcon = 13;  // grid 1

//         29      28                         15     14
//       ------- -------                   ------- -------
//   30 |  \  26|    /  | 24           16 |  \  12|    /  | 10
//      | 27\   |   /25 |                 | 13\   |   /11 |
//      |    \  |  /    |                 |    \  |  /    |
//   22  ------- -------  23            8  ------- -------  9
//      |    /  |  \    |                 |    /  |  \    |
//      | 20/   |   \18 |                 | 6 /   |   \4  |
//   21 |  /  19|    \  | 17            7 |  /   5|    \  | 3
//       ------- -------                   ------- -------
//         16      15                         2        1
//
//   1234567890123456
static const char VFDdigits[36][17] = {
    "1110001001000111",  // 0
    "0010000001000000",  // 1
    "1100001111000110",  // 2
    "1110000111000110",  // 3
    "0010000111000001",  // 4
    "1110000110000111",  // 5
    "1110001110000111",  // 6
    "0010000001000110",  // 7
    "1110001111000111",  // 8
    "0010000111000111",  // 9
    "0010001111000111",  // A
    "1110100011010110",  // B
    "1100001000000111",  // C
    "1110100001010110",  // D
    "1100001100000111",  // E
    "0000001100000111",  // F
    "1110001010000111",  // G
    "0010001111000001",  // H
    "1100100000010110",  // I
    "1110001001000000",  // J
    "0001001100100001",  // K
    "1100001000000001",  // L
    "0010001001101001",  // M
    "0011001001001001",  // N
    "1110001001000111",  // O
    "0000001111000111",  // P
    "1111001001000111",  // Q
    "0001001111000111",  // R
    "1110000010001110",  // S
    "0000100000010110",  // T
    "1110001001000001",  // U
    "0000011000100001",  // V
    "0011011001000001",  // W
    "0001010000101000",  // X
    "0000100000101000",  // Y
    "1100010000100110"   // Z
};

static const char VFDgridBlank[12] = "00000000000";
static const char VFDspace[17] = "0000000000000000";

struct ATIMERS {

  unsigned long pauseMillis = 1;
  unsigned long rotaryPause = 5;
 
  unsigned long currentMillis = 0;
  unsigned long previousMillis = 0;
  unsigned long rotaryTime = 0;
  int rotAprev=0;
  int previousSecond = 0;
  int previousHour = 0;
  boolean ntpTimeSet = false;
  int dateDisplaySec = 0; 
  boolean showDate = false;
};

// https://www.gov.uk/when-do-the-clocks-change
struct DST {
  char currentDST[4] = "XXX";
  int offset = 0;
  time_t BST;
  time_t GMT;
  int currentYear = 0;  
  boolean dset = false;
};

struct VFD {  
  int grids = 0;
  char out[12][33];
  char blank[33];
  char displayOut[12];
  char lookup[35][33];
  boolean showClockIcon=false;
};



