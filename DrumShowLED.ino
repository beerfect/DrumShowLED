//ПОДКЛЮЧЕНИЕ БИБЛИОТЕКИ
#include <FastLED.h>    //подключаем библиотеку
#define NUM_LEDS 180    // общее количество светодиодов
#define DATA_PIN 8      // пин подключенный к ленте
CRGB leds[NUM_LEDS];    // массив для манипуляции светодиодами

// ПОДКЛЮЧЕНИЕ ДАТЧИКОВ
const int sensorPinA = 3; // Куда подключен датчик А
const int sensorPinB = 4; // Куда подключен датчик B
const int sensorPinC = 5; // Куда подключен датчик C

// ЦВЕТ, ЯРКОСТЬ, ЗАТЕМНЕНИЕ
int colorA = 96;          // дефолтный цвет отрезка 1
int colorB = 0;           // дефолтный цвет отрезка 2
int colorC = 160;         // дефолтный цвет отрезка 3
float brightnessA = 0;    // дефолтная яркость A
float brightnessB = 0;    // дефолтная яркость B
float brightnessC = 0;    // дефолтная яркость C
float blackoutStep = 3;   //доступные шаги затемнения: 0.5, 1.5, 1, 3, 5, 15, 17, 42.5

//ПЕРЕКЛЮЧЕНИЕ РЕЖИМОВ
const int modeChangePin = 2; // Куда подключена кнопка смены режимов
int currentMode = 0;         // переменная для переключения режимов
const byte rPin = 13;        // Пины индикатора переключения режимов
const byte gPin = 12;        // Пины индикатора переключения режимов
const byte bPin = 11;        // Пины индикатора переключения режимов

// БЕЛЫЙ РЕЖИМ
const int whiteSwitcherPin = 16; // Куда подключена кнопка переключения белого режима
const byte whiteSwitchPinR = 17; // Пины индикатора переключения белого режима
const byte whiteSwitchPinG = 18; // Пины индикатора переключения белого режима
const byte whiteSwitchPinB = 19; // Пины индикатора переключения белого режима
bool isWhite = true;             // переменная для переключения белого цвета светодиодов
int saturation;

//РАСЧЕТЫ
int partLength = NUM_LEDS / 3;            // количество светодиодов у каждого барабана
int startPartA = 0;                       // начало первого отрезка
int startPartB = startPartA + partLength; // начало второго отрезка
int startPartC = NUM_LEDS - partLength;   // начало третьего отрезка

void setup() {
  // ДЕКЛАРИРУЕМ БИБЛИОТЕКЕ С ЧЕМ ЕЙ РАБОТАТЬ
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  

  // ДАТЧИКИ
  pinMode(sensorPinA, INPUT);
  pinMode(sensorPinB, INPUT);
  pinMode(sensorPinC, INPUT);

  //ПЕРЕКЛЮЧЕНИЕ РЕЖИМОВ
  pinMode(modeChangePin, INPUT);   // Вход переключателя режимов
  pinMode( rPin, OUTPUT );         // Выход индикатора режима
  pinMode( gPin, OUTPUT );         // Выход индикатора режима
  pinMode( bPin, OUTPUT );         // Выход индикатора режима

  //БЕЛЫЙ РЕЖИМ
  pinMode(whiteSwitcherPin, INPUT);           // Вход переключателя белого режима
  pinMode( whiteSwitchPinR, OUTPUT );         // Выход индикатора белого режима
  pinMode( whiteSwitchPinG, OUTPUT );         // Выход индикатора белого режима
  pinMode( whiteSwitchPinB, OUTPUT );         // Выход индикатора белого режима
}

void loop() {

  // Постепенно затемняем все светодиоды у которых не нулевая яркость
  brightnessA -= blackoutStep;
  if (brightnessA < 0) {
    brightnessA = 0;
  }

  brightnessB -= blackoutStep;
  if (brightnessB < 0) {
    brightnessB = 0;
  }

  brightnessC -= blackoutStep;
  if (brightnessC < 0) {
    brightnessC = 0;
  }

  fill_solid(&(leds[startPartA]), partLength, CHSV(colorA, saturation, brightnessA ));
  fill_solid(&(leds[startPartB]), partLength, CHSV(colorB, saturation, brightnessB ));
  fill_solid(&(leds[startPartC]), partLength, CHSV(colorC, saturation, brightnessC ));
  FastLED.show();

  //  ПЕРЕКЛЮЧАТЕЛЬ РЕЖИМОВ
  if (digitalRead(modeChangePin)) {
    currentMode += 1;
    if (currentMode > 2) {
      currentMode = 0;
    }
    while (digitalRead(modeChangePin)) {
    }
  }

  // Режим 0 (RED indication)
  if (currentMode == 0) {
    digitalWrite( bPin, LOW ); digitalWrite( rPin, HIGH );

    if (digitalRead(sensorPinB)) {
      brightnessA = 255;
      brightnessB = 255;
      brightnessC = 255;
    }
  }
  // Режим 1 (GREEN indication)
  if (currentMode == 1) {
    digitalWrite( rPin, LOW ); digitalWrite( gPin, HIGH );

    if (digitalRead(sensorPinA)) {
      brightnessA = 255;
      brightnessC = 255;
    }
    if (digitalRead(sensorPinB)) {
      brightnessB = 255;
    }
  }
  //Режим 2 (BLUE indication)
  if (currentMode == 2) {
    digitalWrite( gPin, LOW ); digitalWrite( bPin, HIGH );
    if (digitalRead(sensorPinA)) {
      brightnessA = 255;
    }
    if (digitalRead(sensorPinB)) {
      brightnessB = 255;
    }
    if (digitalRead(sensorPinC)) {
      brightnessC = 255;
    }
  }

  // ПЕРЕКЛЮЧАТЕЛЬ БЕЛОГО ЦВЕТА
  if (digitalRead(whiteSwitcherPin)) {
    isWhite = !isWhite;
    while (digitalRead(whiteSwitcherPin)) {}
  }

  if (isWhite == 0) {
    digitalWrite( whiteSwitchPinR, LOW );
    digitalWrite( whiteSwitchPinG, LOW );
    digitalWrite( whiteSwitchPinB, LOW );
    saturation = 255;
  }
  if (isWhite == 1) {
    digitalWrite( whiteSwitchPinR, HIGH );
    digitalWrite( whiteSwitchPinG, HIGH );
    digitalWrite( whiteSwitchPinB, HIGH );
    saturation = 0;
  }
}