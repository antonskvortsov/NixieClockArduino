// Бибилиотека разработана для RTC модуля http://iarduino.ru/shop/Expansion-payments/chasy-realnogo-vremeni-rtc-trema-modul.html
// Данный пример устанавливает на модуле заданное время 


#include <iarduino_RTC.h>
iarduino_RTC time(RTC_DS1307);
void setup() {
    delay(300);
    Serial.begin(9600);
    time.begin();
    time.settime(0,51,21,27,10,15,2);  // 0  сек, 51 мин, 21 час, 27, октября, 2015 года, вторник
}
void loop(){
    if(millis()%1000==0){ // если прошла 1 секунда
      Serial.println(time.gettime("d-m-Y, H:i:s, D")); // выводим время
      delay(1); // приостанавливаем на 1 мс, чтоб не выводить время несколько раз за 1мс
    }
}

//    ОПИСАНИЯ ПАРАМЕТРОВ ФУНКЦИЙ:
//
//  Подключение библиотеки:
//  #include <iarduino_RTC.h>
//  iarduino_RTC time(название модуля [, вывод SS/RST [, вывод CLK [, вывод DAT]]]);
//      если модуль работает на шине I2C или SPI, то достаточно указать 1 параметр, например: iarduino_RTC time(RTC_DS3231);
//      если модуль работает на шине SPI, а аппаратный вывод SS занят, то номер назначенного вывода SS для модуля указывается вторым параметром, например: iarduino_RTC time(RTC_DS1305,22);
//      если модуль работает на трехпроводной шине, то указываются номера всех выводов, например: iarduino_RTC time(RTC_DS1302, 1, 2, 3); // RST, CLK, DAT
//  
//  Для работы с модулями, в библиотеке реализованы 5 функции:
//      инициировать модуль  begin();
//      указать время        settime(секунды [, минуты [, часы [, день [, месяц [, год [, день недели]]]]]]);
//      получить время       gettime("строка с параметрами");
//      мигать времем        blinktime(0-не_мигать / 1-мигают_сек / 2-мигают_мин / 3-мигают_час / 4-мигают_дни / 5-мигают_мес / 6-мигает_год / 7-мигают_дни_недели / 8-мигает_полдень)
//      разгрузить шину      period (минуты);
//  
//  Функция begin():
//      функция инициирует модуль: проверяет регистры модуля, запускает генератор модуля и т.д.
//  
//  Функция settime(секунды [, минуты [, часы [, день [, месяц [, год [, день недели]]]]]]):
//      записывает время в модуль
//      год указывается без учёта века, в формате 0-99
//      часы указываются в 24-часовом формате, от 0 до 23
//      день недели указывается в виде числа: 0-воскресенье, 1-понедельник, 2-вторник ..., 6-суббота
//      если предыдущий параметр надо оставить без изменений, то можно указать отрицательное или заведомо большее значение
//      пример: time.settime(-1, 10); установит 10 минут, а секунды, часы и дату, оставит без изменений
//      пример: time.settime(0, 5, 13); установит 13 часов, 5 минут, 0 секунд, а дату оставит без изменений
//      пример: time.settime(-1, -1, -1, 9, 2, 17); установит дату 09.02.2017 , а время и день недели оставит без изменений
//  
//  Функция gettime("строка с параметрами"):
//      функция получает и выводит строку заменяя описанные ниже символы на текущее время
//      пример: time.gettime("d-m-Y, H:i:s, D"); ответит строкой "01-10-2015, 14:00:05, Thu"
//      пример: time.gettime("s");               ответит строкой "05"
//      указанные символы идентичны символам для функции date() в PHP
//  s   секунды                       от      00    до       59  (два знака)
//  i   минуты                        от      00    до       59  (два знака)
//  h   часы в 12-часовом формате     от      01    до       12  (два знака)
//  H   часы в 24-часовом формате     от      00    до       23  (два знака)
//  d   день месяца                   от      01    до       31  (два знака)
//  w   день недели                   от       0    до        6  (один знак: 0-воскресенье, 6-суббота)
//  D   день недели наименование      от     Mon    до      Sun  (три знака: Mon Tue Wed Thu Fri Sat Sun)
//  m   месяц                         от      01    до       12  (два знака)
//  M   месяц наименование            от     Jan    до      Dec  (три знака: Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec)
//  Y   год                           от    2000    до     2099  (четыре знака)
//  y   год                           от      00    до       99  (два знака)
//  a   полдень                               am   или       pm  (два знака, в нижнем регистре)
//  A   полдень                               AM   или       PM  (два знака, в верхнем регистре)
//  
//  если требуется получить время в виде цифр, то можно вызвать функцию gettime() без параметра, после чего получить время из переменных
//      пример:  time.gettime();
//               Serial.print(time.Hours);   Serial.print(":");  // Вывод часов
//               Serial.print(time.minutes); Serial.print(":");  // Вывод минут
//               Serial.print(time.seconds); Serial.println(""); // Вывод секунд
//      seconds  секунды     0-59
//      minutes  минуты      0-59
//      hours    часы        1-12
//      Hours    часы        0-23
//      midday   полдень     0-1 (0-am, 1-pm)
//      day      день месяца 1-31
//      weekday  день недели 0-6 (0-воскресенье, 6-суббота)
//      month    месяц       1-12
//      year     год         0-99
//  
//  Функция blinktime(параметр [, частота ]):
//      указывает функции gettime мигать одним из параметров времени (заменять параметр пробелами)
//      функция может быть полезна, для отображения на дисплее, устанавливаемого параметра времени
//      например при установке минут, они начинают мигать, и Вам понятно, что именно Вы устанавливаете
//      первым аргументом функции указывается параметр в виде числа от 0 до 8
//  0   не мигать
//  1   мигают сек
//  2   мигают мин
//  3   мигают час
//  4   мигают дни
//  5   мигают мес
//  6   мигает год
//  7   мигают дни недели
//  8   мигает полдень
//      второй аргумент функции является необязательным, он указвает частоту миганий в Гц, по умолчанию 1Гц
//      пример: time.blinktime(6);    // при выводе на дисплеё будет мигать год с частотой по умолчанию 1Гц
//      пример: time.blinktime(6, 2); // при выводе на дисплеё будет мигать год с частотой 2Гц
//  
//  Функция period(минуты):
//      устанавливает минимальный период обращения к модулю в минутах (от 0 до 255)
//      функция может быть полезна, если шина сильно нагружена, на ней имеются несколько устройств
//      пример: time.period(10); // теперь каждые 10 минут к модулю может быть отправлен только 1 запрос на получение времени
//      ответом на все остальные запросы будет результат последнего полученного от модуля времени + время прошедшее с этого запроса