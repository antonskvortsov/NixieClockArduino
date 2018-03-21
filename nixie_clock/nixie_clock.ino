#include <DS3231.h>
//#include <Time.h>
#include <LiquidCrystal.h>

#include <iarduino_RTC.h>               // подключаем библиотеку для работы с RTC модулем


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Init the DS3231 using the hardware interface
//DS3231 rtc(SDA, SCL);
iarduino_RTC time(RTC_DS3231);          // объявляем переменную time для работы с библиотекой, указывая название модуля RTC_DS1307
//Time dt;

// constants won't change. They're used here to
// set pin numbers:

const uint8_t PIN_button_SET  = 9;           // указываем номер вывода arduino, к которому подключена кнопка SET
const uint8_t PIN_button_UP   = 8;           // указываем номер вывода arduino, к которому подключена кнопка UP
const uint8_t PIN_button_DOWN = 7;           // указываем номер вывода arduino, к которому подключена кнопка DOWN
uint8_t VAR_mode_SHOW   = 1;           // режим вывода: 1-время 2-дата
uint8_t VAR_mode_SET    = 0;           // режим установки времени: 0-нет 1-сек 2-мин 3-час 4-день 5-мес 6-год 7-день_недели
const int ledPin =  13;      // the number of the LED pin

const char *modeName[8] = {
  "-", "sec", "min", "hou", "day", "mon",
  "yea", "dow"
};

// выводы на ИД1 (1-2-4-8)
int Pin_num[4] = {5, 4, 3, 2};
//Массив времени (десятки часа,единицы часа,десятки минут и т.д....)
int NumberColb[6] = {0, 0, 0, 0, 0, 0};

//Массив с помощью которого дешефратору задаются цифры
static const uint8_t numbers[11][4] =
{
  { 0, 0, 0, 0 }, //0
  { 1, 0, 0, 0 }, //1
  { 0, 1, 0, 0 }, //2
  { 1, 1, 0, 0 }, //3
  { 0, 0, 1, 0 }, //4
  { 1, 0, 1, 0 }, //5
  { 0, 1, 1, 0 }, //6
  { 1, 1, 1, 0 }, //7
  { 0, 0, 0, 1 }, //8
  { 1, 0, 0, 1 }, //9
  { 1, 1, 1, 1 }  //Чисто
};

unsigned long currentMillis = 0;
long previousMillis = 0;        // храним время последнего показа даты
long interval = 20000;           // интервал между включение/выключением даты
long intervalCatod = 600000;  // интервал перебора цифр от отравления
long otrMillis = 0; // храним время последнего пробега от отравления
long catodMillis = 0; // храним время задержки перебора
long setMillis = 0; // храним время последнего нажатия при установке времени даты
byte static prevSecond = 0; // тут будем хранить, сколько секунд было при прошлом отчете
boolean but = false; // выход из установки времени
boolean bat = false; // выход из установки времени по истечении 8 сек

void setup()
{
  pinMode(PIN_button_SET,  INPUT);           // устанавливаем режим работы вывода кнопки, как "вход"
  pinMode(PIN_button_UP,   INPUT);           // устанавливаем режим работы вывода кнопки, как "вход"
  pinMode(PIN_button_DOWN, INPUT);           // устанавливаем режим работы вывода кнопки, как "вход"
  delay(300);                                // ждем 300мс

  // Initialize the rtc object
  //  rtc.begin();                                      // инициируем RTC модуль
  time.begin();                         // инициируем RTC модуль
  lcd.begin(16, 2);                                // инициируем LCD дисплей

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);


  // Setup Serial connection
  Serial.begin(9600);
  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}

  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // The following lines can be uncommented to set the date and time
  //      rtc.setDOW(4);     // Set Day-of-Week to SUNDAY
  //      rtc.setTime(23, 24, 0);     // Set the time to 12:00:00 (24hr format)
  //      rtc.setDate(4, 5, 2017);   // Set the date to January 1st, 2014
}

void loop()
{
  currentMillis = millis();


  if (millis() % 1000 == 0) {                // если прошла 1 секунда
    if (VAR_mode_SHOW == 1) {                // если установлен режим вывода времени
      aTime();
      if (millis() % 3000 == 0) {
        Zazhig();
      }
      lcd.setCursor(0, 0);                   // устанавливаем курсор в позицию: столбец 0, строка 0
      lcd.print("Time ");              // выводим текст ""
      lcd.print(modeName[VAR_mode_SET]);
      lcd.setCursor(0, 1);                   // устанавливаем курсор в позицию: столбец 0, строка 1
      lcd.print(time.gettime("H:i:s"));      // выводим время
    }
    if (VAR_mode_SHOW == 2) {                // если установлен режим вывода даты
      aDate();
      if (millis() % 3000 == 0) {
        Zazhig();
      }
      lcd.setCursor(0, 0);                   // устанавливаем курсор в позицию: столбец 0, строка 0
      lcd.print("Date ");              // выводим текст ""
      lcd.print(modeName[VAR_mode_SET]);
      lcd.setCursor(0, 1);                   // устанавливаем курсор в позицию: столбец 0, строка 1
      lcd.print(time.gettime("d.m.y   D"));  // выводим дату
    }
    //    if (VAR_mode_SHOW == 1) {                // если установлен режим вывода времени
    //      lcd.print(rtc.getTimeStr());      // выводим время
    //    }
    //    if (VAR_mode_SHOW == 2) {                // если установлен режим вывода даты
    //      lcd.print(rtc.getDateStr());  // выводим дату
    //    }
    delay(1);                                // приостанавливаем на 1 мс, чтоб не выводить время несколько раз за 1мс

  }

  Func_buttons_control();                    // передаём управление кнопкам

  if (millis() % 10000 == 0 && VAR_mode_SET == 0) {
    VAR_mode_SHOW = (VAR_mode_SHOW++ % 2) + 1;
    lcd.clear();
  }




  // Send Day-of-Week
  //  Serial.print(rtc.getDOWStr());
  //  Serial.print(" ");
  //
  //  // Send date
  //  Serial.print(rtc.getDateStr());
  //  Serial.print(" -- ");
  //
  //  // Send time
  //  Serial.println(rtc.getTimeStr());

  //  lcd.print(millis() / 1000);

  // lcd.print(rtc.getDOWStr());
  // lcd.print(" ");
  //  lcd.setCursor(0, 0);
  //  lcd.print(rtc.getDateStr());
  //  //  lcd.print(" ");
  //  lcd.setCursor(0, 1);
  //  lcd.print(rtc.getTimeStr());
  //  //  lcd.print(".");
  //  //  lcd.print(millis());
  //
  // Wait one second before repeating :)
  //  delay (1000);
}

void aTime() {
  // часы; загоняем текущее время побитово в массив NumberColb

  int h = time.Hours;
  int cel = 0;
  while (h >= 10) {
    cel++;
    h -= 10;
  }
  NumberColb[0] = cel;  // десятки часа
  NumberColb[1] = h;    // еденицы часа

  h = time.minutes;
  cel = 0;
  while (h >= 10) {
    cel++;
    h -= 10;
  }
  NumberColb[2] = cel;
  NumberColb[3] = h;

  h = time.seconds;
  cel = 0;
  while (h >= 10) {
    cel++;
    h -= 10;
  }
  NumberColb[4] = cel;
  NumberColb[5] = h;
  prevSecond = time.seconds;
  //
  //  analogWrite(Red, 255);
  //  analogWrite(Green, 255);
  //  analogWrite(Blue, tCol[0][TCol]);
}

void aDate() {
  // загоняем дату побитово в массив NumberColb
  int h = time.day;
  int cel = 0;
  while (h >= 10) {
    cel++;
    h -= 10;
  }
  NumberColb[0] = cel;  // десятки дня
  NumberColb[1] = h;    // еденицы дня

  h = time.month;
  cel = 0;
  while (h >= 10) {
    cel++;
    h -= 10;
  }
  NumberColb[2] = cel;
  NumberColb[3] = h;

  h = time.year;
  cel = 0;
  while (h >= 10) {
    cel++;
    h -= 10;
  }
  NumberColb[4] = cel;
  NumberColb[5] = h;

  //  Db = false;

  //analogWrite(Red, 255);
  //analogWrite(Green, tCol[0][TCol]+20);
  //analogWrite(Blue, 255);
  //
}

void Zazhig() {

  // Send date
  for (int index = 0; index < 6; index++) {
    Serial.print(NumberColb[index]);

  }
  Serial.println();

  //  // вывод времени на лампы
  //  for (int i = 0; i < 6; i++) {
  //
  //    for (int k = 0; k < 4; k++) {
  //      digitalWrite (Pin_num [k], numbers [NumberColb [i]] [k]); // загоняем время в ИД1
  //    }
  //    // если нет данных в регистре выключаем анод
  //    if (NumberColb [i] == 11) {
  //      digitalWrite(latchPinA, LOW);
  //      shiftOut(dataPinA, clockPinA, LSBFIRST, color);
  //      shiftOut(dataPinA, clockPinA, LSBFIRST, 0);
  //      digitalWrite(latchPinA, HIGH);
  //    }
  //    else {
  //      // зажигаем поочередно лампы со светодиодами
  //      digitalWrite(latchPinA, LOW);
  //      shiftOut(dataPinA, clockPinA, LSBFIRST, color);
  //      shiftOut(dataPinA, clockPinA, LSBFIRST, tanode [i]);
  //      digitalWrite(latchPinA, HIGH);
  //    }
  //
  //    delayMicroseconds (tOn[TOn]);// время свеченя лампы
  //
  //    // гасим анод (убираем засветку предыдущего разряда )
  //    digitalWrite(latchPinA, LOW);
  //    shiftOut(dataPinA, clockPinA, LSBFIRST, color);
  //    shiftOut(dataPinA, clockPinA, LSBFIRST, 0);
  //    digitalWrite(latchPinA, HIGH);
  //    delayMicroseconds (Gas);
  //  }
}

// перебор от отравления
//void Catodmillis() {
//  if (podsvet == true) {
//    color = 255;
//  }
//  else {
//    color = 0;
//  }
//  analogWrite(Red, 255);
//  analogWrite(Green, tCol[1][TCol]);
//  analogWrite(Blue, tCol[1][TCol]);
//
//  for (int w = 0; w < 10; w++) {
//
//    for (int u = 0; u < 6; u++) {
//      NumberColb[u] = w;
//    }
//    while (currentMillis - catodMillis < 150) {
//      currentMillis = millis();
//      Zazhig();
//    }
//    catodMillis = currentMillis;
//  }
//
//}

void Func_buttons_control() {
  uint8_t i = 0;
  time.blinktime(VAR_mode_SET);                                  // мигаем устанавливаемым параметром (если VAR_mode_SET больше 0)
  //Если часы находятся в режиме установки даты/времени
  if (VAR_mode_SET) {
    //  Если нажата кнопка UP

    digitalWrite(ledPin, HIGH);

    if (digitalRead(PIN_button_UP  )) {
      while (digitalRead(PIN_button_UP)) {
        delay(50); // ждём пока мы не отпустим кнопку UP
      }
      switch (VAR_mode_SET) {                                    // инкремент (увеличение) устанавливаемого значения
      /* сек */ case 1: time.settime(0,                                   -1, -1, -1, -1, -1, -1); break;
      /* мин */ case 2: time.settime(-1, (time.minutes == 59 ? 0 : time.minutes + 1), -1, -1, -1, -1, -1); break;
      /* час */ case 3: time.settime(-1, -1, (time.Hours == 23 ? 0 : time.Hours + 1),     -1, -1, -1, -1); break;
      /* дни */ case 4: time.settime(-1, -1, -1, (time.day == 31 ? 1 : time.day + 1),         -1, -1, -1); break;
      /* мес */ case 5: time.settime(-1, -1, -1, -1, (time.month == 12 ? 1 : time.month + 1),     -1, -1); break;
      /* год */ case 6: time.settime(-1, -1, -1, -1, -1, (time.year == 99 ? 0 : time.year + 1),       -1); break;
      /* д.н.*/ case 7: time.settime(-1, -1, -1, -1, -1, -1, (time.weekday == 6 ? 0 : time.weekday + 1) ); break;
      }
    }
    //  Если нажата кнопка DOWN
    if (digitalRead(PIN_button_DOWN)) {
      while (digitalRead(PIN_button_DOWN)) {
        delay(50); // ждём пока мы её не отпустим
      }
      switch (VAR_mode_SET) {                                    // декремент (уменьшение) устанавливаемого значения
      /* сек */ case 1: time.settime(0,                                   -1, -1, -1, -1, -1, -1); break;
      /* мин */ case 2: time.settime(-1, (time.minutes == 0 ? 59 : time.minutes - 1), -1, -1, -1, -1, -1); break;
      /* час */ case 3: time.settime(-1, -1, (time.Hours == 0 ? 23 : time.Hours - 1),     -1, -1, -1, -1); break;
      /* дни */ case 4: time.settime(-1, -1, -1, (time.day == 1 ? 31 : time.day - 1),         -1, -1, -1); break;
      /* мес */ case 5: time.settime(-1, -1, -1, -1, (time.month == 1 ? 12 : time.month - 1),     -1, -1); break;
      /* год */ case 6: time.settime(-1, -1, -1, -1, -1, (time.year == 0 ? 99 : time.year - 1),       -1); break;
      /* д.н.*/ case 7: time.settime(-1, -1, -1, -1, -1, -1, (time.weekday == 0 ? 6 : time.weekday - 1) ); break;
      }
    }
    //  Если нажата кнопка SET
    if (digitalRead(PIN_button_SET)) {
      while (digitalRead(PIN_button_SET)) {                      // ждём пока мы её не отпустим
        delay(10);
        if (i < 200) {
          i++; // фиксируем, как долго удерживается кнопка SET, если дольше 2 секунд, то стираем экран
        } else {
          lcd.clear();
        }
      }
      if (i < 200) {                                             // если кнопка SET удерживалась меньше 2 секунд
        VAR_mode_SET++;                                          // переходим к следующему устанавливаемому параметру
        if (VAR_mode_SHOW == 1 && VAR_mode_SET > 3) {
          VAR_mode_SET = 1; // возвращаемся к первому устанавливаемому параметру
        }
        if (VAR_mode_SHOW == 2 && VAR_mode_SET > 7) {
          VAR_mode_SET = 4; // возвращаемся к первому устанавливаемому параметру
        }
      } else {                                                    // если кнопка SET удерживалась дольше 2 секунд, то требуется выйти из режима установки даты/времени
        VAR_mode_SET = 0;                                        // выходим из режима установки даты/времени
      }
    }
    //Если часы находятся в режиме вывода даты/времени
  } else {
    //  Если нажата кнопка SET

    digitalWrite(ledPin, LOW);

    if (digitalRead(PIN_button_SET)) {
      while (digitalRead(PIN_button_SET)) {
        delay(10);              // ждём пока мы её не отпустим
        if (i < 200) {
          i++; // фиксируем, как долго удерживается кнопка SET, если дольше 2 секунд, то стираем экран
        } else {
          lcd.clear();
        }
      }
      if (i < 200) {                                             // если кнопка SET удерживалась меньше 2 секунд
        lcd.clear();                                             // стираем экран
        VAR_mode_SHOW = VAR_mode_SHOW == 1 ? 2 : 1;              // меняем режим вывода: дата/время
      } else {                                                    // если
        if (VAR_mode_SHOW == 1) {
          VAR_mode_SET = 1; // если установлен режим вывода времени, то устанавливаемый параметр - секунды
        }
        if (VAR_mode_SHOW == 2) {
          VAR_mode_SET = 4; // если установлен режим вывода даты,    то устанавливаемый параметр - день
        }
      }
    }
  }
}
