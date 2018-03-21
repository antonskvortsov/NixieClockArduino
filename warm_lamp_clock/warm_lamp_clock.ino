#include <Time.h>
#include <TimeLib.h>

#include <DS1307RTC.h>


const int ledPin = 9;
const int inputPin = 2;

//словарь цифр
char dictionary[10][4] = {
  {1, 0, 0, 1},//0
  {0, 0, 0, 1},//1
  {0, 0, 0, 0},//2
  {1, 0, 1, 0},//3
  {0, 1, 1, 0},//4
  {1, 1, 0, 0},//5
  {0, 1, 0, 0},//6
  {1, 1, 1, 0},//7
  {0, 0, 1, 0},//8
  {1, 0, 0, 0},//9
};

// выводы для дешифратора
//defPin[out1,out2,out4,out8];
int defPin[] = {A0, A2, A1, A3};

// выводы для транзисторных ключей
int keysPin[] = {12, 8, 7, 6, 0, 1};

//массив для состояние кнопок
int buttonPin = 13;
int buttonsStates[] = {0, 0, 0, 0, 0, 0};
int inputPinState = 0;

//переменные для времени
//Time t;
int TimeDate[] = {0, 0, 0, 0, 0, 0}; // массив где хранится время или дата
int Lamps[] = {0, 0, 0, 0, 0, 0};

//переменные для меню
//если == true : Time, else Date
bool TimeDateFlag = true;

bool settingTimeDateFlag = false;
long but1 = 0;

//переменные для счетчиков
int v = 0, p = 7, k = 0, lock = 0;

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) ; // Только для платы Leonardo
  setSyncProvider(RTC.get);   // получаем время с RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC"); //синхронизация не удаласть
  else
    Serial.println("RTC has set the system time");

  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);

}

void loop() {

  int val = digitalRead(inputPin);
  if (val == HIGH)
  {
    digitalWrite(ledPin, HIGH);
    //    digitalClockDisplay();
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }


  // put your main code here, to run repeatedly:
  k++;//счетчик для мигания цифрами
  v++;//счетчик для пебора всех цифр
  //  setColor();
  checkTimeDate();
  buttonState();
  if (v >= 32760) {//делам прогон всех цифр во избежание "Отравление" катодов ламп
    scrollLampsNumbers(25);
    v = 0;
  }
  showLamps(Lamps);
  //  Serial.println(v);


}

void digitalClockDisplay() {
  // digital clock display of the time
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
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

//вывод времени для каждой лампы
void checkTimeDate() {
  tmElements_t tm1;

  if (RTC.read(tm1)) {
    if (!settingTimeDateFlag) {//если вышли из режима настройки
      int temp;
      if (TimeDateFlag) {//если мы отображали время то и будем настраивать время иначе будем настраивать дату
        Lamps[0] = tm1.Hour / 10;
        Lamps[1] = tm1.Hour % 10;
        Lamps[2] = tm1.Minute / 10;
        Lamps[3] = tm1.Minute % 10;
        Lamps[4] = tm1.Second / 10;
        Lamps[5] = tm1.Second % 10;
      }
      else {
        Lamps[0] = tm1.Day / 10;
        Lamps[1] = tm1.Day % 10;
        Lamps[2] = tm1.Month / 10;
        Lamps[3] = tm1.Month % 10;
        temp = tmYearToY2k(tm1.Year);
        Lamps[4] = temp / 10;
        Lamps[5] = temp % 10;
      }
    }
    //  t = setSyncProvider(RTC.get);   // получаем время с RTC
  }
}

//обновления времени и даты
void updateTimeDate() {
  tmElements_t tm1;

  if (RTC.read(tm1)) {
    //    int temp;
    if (TimeDateFlag) {
      //    rtc.setTime(TimeDate[0] * 10 + TimeDate[1], TimeDate[2] * 10 + TimeDate[3], TimeDate[4] * 10 + TimeDate[5]);
      tm1.Hour =   Lamps[0] * 10 + Lamps[1];  // mask assumes 24hr clock
      tm1.Minute = Lamps[2] * 10 + Lamps[3];
      tm1.Second = Lamps[4] * 10 + Lamps[5];
      //  tm1.Wday = bcd2dec(Wire.read() );
      tm1.Day = day();
      tm1.Month = month();
      tm1.Year = CalendarYrToTm(year());

      RTC.write(tm1);
      //      digitalClockDisplay();

      //      Serial.println(bcd2dec(tm1.Hour), bcd2dec(tm1.Minute), bcd2dec(tm1.Second), bcd2dec(tm1.Day), bcd2dec(tm1.Month), bcd2dec(tm1.Year))
    }
    else {
      //      rtc.setDate(Lamps[0] * 10 + TimeDate[1], TimeDate[2] * 10 + TimeDate[3], 2000 + TimeDate[4] * 10 + TimeDate[5]);
      tm1.Hour =   hour();  // mask assumes 24hr clock
      tm1.Minute = minute();
      tm1.Second = second();
      //  tm1.Wday = bcd2dec(Wire.read() );
      tm1.Day = Lamps[0] * 10 + Lamps[1];
      tm1.Month = Lamps[2] * 10 + Lamps[3];
      tm1.Year = y2kYearToTm(Lamps[4] * 10 + Lamps[5]);

      RTC.write(tm1);
      //      digitalClockDisplay();
      //      Serial.println(bcd2dec(tm1.Hour), bcd2dec(tm1.Minute), bcd2dec(tm1.Second), bcd2dec(tm1.Day), bcd2dec(tm1.Month), bcd2dec(tm1.Year))
    }
  }
}

void showLamps(int a[])
{
  for (int i = 0; i < 6; i++)
  {
    //передаем сигналы для a[i] цифры
    setNumber(a[i]);
    //    digitalWrite(keysPin[i], HIGH);//подаем сигнал на keysPin[i] индикатор
    //задержка между лампами
    if (!((i == p || i == p - 1) && (k > 50))) {
      delay(2);
    }
    //опрос кнопок i-й кнопки
    buttonsStates[i] = digitalRead(buttonPin);
    inputPinState = digitalRead(inputPin);

    //потушим keysPin[i] индикатор
    //    digitalWrite(keysPin[i], LOW);

    //задержка для мигания отдельных разрядов
    if ((i == p) && (k > 50)) {
      delay(2);
    }

    if (k > 100) {
      k = 0;
    }
  }
  digitalClockDisplay();
}

//вывод опеределенной цифры
void setNumber(int num) {
  for (int i = 0; i < 4; i++) {//цикл по словарю и взависимости от цифры в словаре подаем сигнал на к155ид1
    //    digitalWrite(defPin[i], ((dictionary[num][i] == 0) ? LOW : HIGH));
    //    Serial.println(defPin[i], ((dictionary[num][i] == 0) ? LOW : HIGH));
  }
}


//меню
void buttonState() {

  if (inputPinState == HIGH) {//организация долгово зажатия 2й и 3й кнопки
    bool doIt = false;
    settingTimeDateFlag  = true;

    Lamps[p]++;
    confPlusButtonDown();//проверка допустимых значение

    digitalClockDisplay();

    if (doIt && settingTimeDateFlag && buttonsStates[2] && !inputPinState) {//кнопка "-"
      Lamps[p]--;
      confMinusButtonDown();//проверка допустимых значение

      digitalClockDisplay();
    }
  }


  if (buttonsStates[1] || buttonsStates[2]) {//организация долгово зажатия 2й и 3й кнопки
    bool doIt = false;
    but1++;//счетчик будет увеличиваться пока кнопка зажата
    if (but1 > 200) {
      if ((but1 % 10) == 0) {
        doIt = true;
      }
    }
    else if (but1 >= 60) {
      if ((but1 % 50) == 0) {
        doIt = true;
      }
    }
    else if (but1 == 4) {
      doIt = true;
    }
    if (doIt && settingTimeDateFlag && buttonsStates[1] && !buttonsStates[2]) { //кнопка "+"
      Lamps[p]++;
      confPlusButtonDown();//проверка допустимых значение
    }
    if (doIt && settingTimeDateFlag && buttonsStates[2] && !buttonsStates[1]) {//кнопка "-"
      Lamps[p]--;
      confMinusButtonDown();//проверка допустимых значение
    }
  }
  if (!lock) {

    if (buttonsStates[0]) {
      if (!settingTimeDateFlag) {//кнопка переключение  между датой и временем
        TimeDateFlag = !TimeDateFlag;
        v = 32760;
      }
      else {//если часы находятся в режиме настройке, то кнопка переключает разряды
        p -= 2;
        if (p < 0) p = 5;
      }
    }
    //кнопка входа в режим настройки
    else if (buttonsStates[3]) {
      p = 5; k = 0;
      settingTimeDateFlag = !settingTimeDateFlag;
      if (!settingTimeDateFlag) {//после того как кнопка была нажата повторна
        updateTimeDate();
        p = 7;
      }
    }
  }
  lock = buttonsStates[0] || buttonsStates[3];//чтобы ардуина считала одно нажатие даже при зажатой кнопки
}

void confPlusButtonDown() {
  int SecYear = Lamps[4] * 10 + Lamps[5];//сек и года
  int MinMon = Lamps[2] * 10 + Lamps[3];//мин и месеца
  int HourDays = Lamps[0] * 10 + Lamps[1];//часы и дни
  //если младший разряд больше 9 то увеличить на 1 старший разряд и младший обнулить
  if (Lamps[p] > 9) {
    Lamps[p - 1]++;
    Lamps[p] = 0;
  }
  if (TimeDateFlag) { //ограничения для времени
    if (HourDays > 23) {
      Lamps[0] = 0; Lamps[1] = 0;
    }
    if (MinMon > 59) {
      Lamps[2] = 0; Lamps[3] = 0;
    }
    if (SecYear > 59) {
      Lamps[4] = 0; Lamps[5] = 0;
    }
  }
  else { //ограничение для даты
    //ограничение по годом не больше 99
    if (SecYear > 99) {
      Lamps[4] = 0; Lamps[5] = 0;
    }
    //ограничение по месецам не больше 12
    if (MinMon > 12) {
      Lamps[2] = 0; Lamps[3] = 0;
    }
    //ограничение для дней не больше 31 или 30 или 29 дней в определенные месяца
    if ((MinMon == 1 || MinMon == 3 || MinMon == 5 || MinMon == 6 || MinMon == 7 || MinMon == 8 || MinMon == 10 || MinMon == 12) && HourDays > 31) {
      Lamps[0] = 0; Lamps[1] = 0;
    }
    else if (( MinMon == 4 || MinMon == 6 || MinMon == 9 || MinMon == 11) && HourDays > 30) {
      Lamps[0] = 0; Lamps[1] = 0;
    }
    else if (MinMon == 2 && HourDays > 29) {
      Lamps[0] = 0; Lamps[1] = 0;
    }
  }
}

void confMinusButtonDown() {
  int SecYear = Lamps[4] * 10 + Lamps[5];//сек и года
  int MinMon = Lamps[2] * 10 + Lamps[3];//мин и месеца
  int HourDays = Lamps[0] * 10 + Lamps[1];//часы и дни
  //если младший разряд мешьше 0 то уменьшить на 1 старший разряд и младший передать 9
  if (Lamps[p] < 0) {
    Lamps[p - 1]--;
    Lamps[p] = 9;
  }
  if (TimeDateFlag) { //ограничения для времени
    if (HourDays < 0) {
      Lamps[0] = 2; Lamps[1] = 3;
    }
    if (MinMon < 0) {
      Lamps[2] = 5; Lamps[3] = 9;
    }
    if (SecYear < 0) {
      Lamps[4] = 5; Lamps[5] = 9;
    }
  }
  else { //ограничения для даты
    //ограничение по годом
    if (SecYear < 0) {
      Lamps[4] = 9; Lamps[5] = 9;
    }
    //ограничение по месецам если меньше нуля по выводить 12й месяц
    if (MinMon < 0) {
      Lamps[2] = 1; Lamps[3] = 2;
    }
    //ограничение для дней не больше 31 или 30 или 29 дней в определенные месяца
    if ((MinMon == 1 || MinMon == 3 || MinMon == 5 || MinMon == 6 || MinMon == 7 || MinMon == 8 || MinMon == 10 || MinMon == 12) && HourDays < 0) {
      Lamps[0] = 3; Lamps[1] = 1;
    }
    else if (( MinMon == 4 || MinMon == 6 || MinMon == 9 || MinMon == 11) && HourDays < 0) {
      Lamps[0] = 3; Lamps[1] = 0;
    }
    else if (MinMon == 2 && HourDays < 0) {
      Lamps[0] = 2; Lamps[1] = 9;
    }
  }
}


//перебор всех цифр
void scrollLampsNumbers(int pause) {
  for (int i = 0; i < 9; i++) {
    int a[] = {i, i, i, i, i, i};
    showLamps(a);
    delay(pause);
  }
}
