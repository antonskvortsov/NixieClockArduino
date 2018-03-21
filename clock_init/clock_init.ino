//формат указания текущего времени "ДД.ММ.ГГ чч:мм:сс"
//где ДД - день, ММ - месяц, ГГ - год, чч - часы, мм - минуты, сс - секунлы
//ГГ - от 00 до 99 для 2000-2099 годов

#include <TimeLib.h>

#include <Time.h>
#include <DS1307RTC.h>

bool isTimeSet = false; //флаг, указывающий на то, была ли уже задана дата

void setup()  {
  Serial.begin(9600);
  while (!Serial) ; // Только для платы Leonardo
  setSyncProvider(RTC.get);   // получаем время с RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC"); //синхронизация не удаласть
  else
    Serial.println("RTC has set the system time");
}

void loop()
{
  if (Serial.available()) { //поступила команда с временем
      setTimeFromFormatString(Serial.readStringUntil('\n'));
      isTimeSet = true; //дата была задана
  }
  if (isTimeSet)  //если была задана дата
  {
    digitalClockDisplay(); //вывод времени
  }
  delay(1000);
}

void digitalClockDisplay() {
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits) {
  //выводим время через ":"
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void setTimeFromFormatString(String time)
{
  //ДД.ММ.ГГ чч:мм:сс
  int day = time.substring(0, 2).toInt();
  int month = time.substring(3, 5).toInt();
  int year = time.substring(6, 8).toInt();
  int hours = time.substring(9, 11).toInt();
  int minutes = time.substring(12, 14).toInt();
  int seconds = time.substring(15, 17).toInt();
  TimeElements te;
  te.Second = seconds;
  te.Minute = minutes;
  te.Hour = hours;
  te.Day = day;
  te.Month = month;
  te.Year = year + 30; //год в библиотеке отсчитывается с 1970. Мы хотим с 2000
  time_t timeVal = makeTime(te);
  RTC.set(timeVal);
  setTime(timeVal);
}
