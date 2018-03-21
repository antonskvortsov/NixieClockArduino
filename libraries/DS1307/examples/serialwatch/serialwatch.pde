
/*
Written by Jurgen Schwietering (scjurgen@yahoo.com)
Example of using DS1307 clock with interrupt driven by SQW of the chip
*/


#include <DS1307.h>
#include <Wire.h>


#define ISRPIN 2 // use pin 2 or 3 for external interrupt

volatile bool newClock=false;
int clockState=0;


// with 1hz squarewave this routine will be called 2 times per second if using CHANGE (attachInterrupt)
// with RISING it will be called 1 time per second
void DS1307Callback()
{
	clockState=digitalRead(ISRPIN);
	newClock=true;
}

void showDateTime()
{
	int rtc[7];
	char outb[256];
	RTC.get(rtc, true);
	sprintf(outb,"%02d:%02d:%02d %2d/%2d/%4d day=%d ",rtc[2],rtc[1],rtc[0],rtc[4],rtc[5],rtc[6],rtc[3]);
	Serial.println(outb);

}

void SetDateTime(int h, int m, int s, int day, int month, int year)
{
	RTC.stop();
	RTC.set(DS1307_HR, h);
	RTC.set(DS1307_MIN, m);
	RTC.set(DS1307_SEC, s);
	RTC.set(DS1307_DATE, day);
	RTC.set(DS1307_MTH, month);
	RTC.set(DS1307_YR, year);
	RTC.start();
	
}

void setup()
{
	Serial.begin(9600);
	Serial.println("setup ISR and stuff");
	
	
	pinMode(ISRPIN,INPUT);
	// pull-up ISRPIN (see DS1307 datasheet)
	digitalWrite(ISRPIN,HIGH); 

	// set external interrupt for 2Hz interruption
	attachInterrupt(ISRPIN-2, DS1307Callback, CHANGE);
	showDateTime();
	RTC.set_ctrlreg(DS1307_1HZ);
	showDateTime();
	//SetDateTime(12,22,0,1,3,11);
	Serial.println("done");
}


void loop()
{
	if (newClock) // show new time only when new interrupt signaled
	{
		newClock=false;
		Serial.print(clockState); // clockstate is useful if you want to do a blinking ':' for the time
		Serial.print(" ");
		showDateTime();
	}
}