/*
  DS1307.h - library for DS1307 rtc
  Created by matt.joyce@gmail.com, December, 2007.
  Released into the public domain.
  
  scjurgen@yahoo.com: 
   added ctrl register functions (set_ctrlreg) to enable clock out with 1Hz, 4Khz, 8Khz or 32Khz (useful with pin 2 or 3 on arduino for precise clock interrupt )
   and preserve seconds function
*/

// ensure this library description is only included once
#ifndef DS1307_h
#define DS1307_h

// include types & constants of Wiring core API

#include <Wire.h>
  
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// include types & constants of Wire ic2 lib

#define DS1307_SEC 0
#define DS1307_MIN 1
#define DS1307_HR 2
#define DS1307_DOW 3
#define DS1307_DATE 4
#define DS1307_MTH 5
#define DS1307_YR 6
#define DS1307_CTRLREG 7

#define DS1307_BASE_YR 2000

#define DS1307_CTRL_ID B1101000  //DS1307

 // Define register bit masks  
#define DS1307_CLOCKHALT B10000000
 
#define DS1307_LO_BCD  B00001111
#define DS1307_HI_BCD  B11110000

#define DS1307_HI_SEC  B01110000
#define DS1307_HI_MIN  B01110000
#define DS1307_HI_HR   B00110000
#define DS1307_LO_DOW  B00000111
#define DS1307_HI_DATE B00110000
#define DS1307_HI_MTH  B00110000
#define DS1307_HI_YR   B11110000


#define DS1307_SQWOFF0 B00000000
#define DS1307_SQWOFF1 B10000000
#define DS1307_1HZ     B10010000
#define DS1307_4KHZ    B10010001
#define DS1307_8KHZ    B10010010
#define DS1307_32KHZ   B10010011

#define DS1307_DATASTART 0x08

// library interface description
class DS1307
{
  // user-accessible "public" interface
  public:
    DS1307();
    void get(int *, boolean);
    int get(int, boolean);
    int min_of_day(boolean);
    void set(int, int);
    void start(void);
    void stop(void);
    void set_ctrlreg(byte);
    void get_sram_data(byte *);
    void set_sram_data(byte *);
    byte get_sram_byte(int);
    void set_sram_byte(byte, int);
    byte getSingleReg(byte reg);
	void setSingleReg(byte reg, byte b);
  // library-accessible "private" interface
  private:
    byte rtc_bcd[7]; // used prior to read/set ds1307 registers;
    void read_rtc(void);
    void save_rtc(void);
};

extern DS1307 RTC;

#endif
