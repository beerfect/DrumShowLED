#include <FastLED.h> //подключаем библиотеку
#define NUM_LEDS 60  // общее количество светодиодов
#define DATA_PIN 8   // пин подключенный к ленте
CRGB leds[NUM_LEDS]; // массив для манипуляции светодиодами

int color1 = 20;  // цвет отрезка 1
int color2 = 120; // цвет отрезка 2
int color3 = 190; // цвет отрезка 3

float brightnessA = 0;    // дефолтная яркость A
float brightnessB = 0;    // дефолтная яркость B
float brightnessC = 0;    // дефолтная яркость C

float blackoutStep = 1; //доступные шаги затемнения: 0.5, 1, 3, 5, 15, 17, 42.5

const int modeChangePin = 2; // Куда подключена кнопка смены режимов

const byte rPin = 13; // Пины индикатора переключения режимов
const byte gPin = 12; // Пины индикатора переключения режимов
const byte bPin = 11; // Пины индикатора переключения режимов

int currentMode = 0;  // переменная для переключения режимов

const int sensorPinA = 3; // Куда подключен сенсор А
const int sensorPinB = 4; // Куда подключен сенсор B
const int sensorPinC = 5; // Куда подключен сенсор C

int partLength = NUM_LEDS / 3;            // количество светодиодов у каждого барабана
int startPartA = 0;                       // начало первого отрезка
int startPartB = startPartA + partLength; // начало второго отрезка
int startPartC = NUM_LEDS - partLength;   // начало третьего отрезка

void setup() {

  pinMode(modeChangePin, INPUT);   // Вход переключателя режимов

  pinMode( rPin, OUTPUT );         // Выход индикатора режима
  pinMode( gPin, OUTPUT );         // Выход индикатора режима
  pinMode( bPin, OUTPUT );         // Выход индикатора режима

  pinMode(sensorPinA, INPUT);      // Вход sensorA
  pinMode(sensorPinB, INPUT);      // Вход sensorB
  pinMode(sensorPinC, INPUT);      // Вход sensorC

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // декларируем библиотеке - к пину DATA_PIN подключены NUM_LEDS диодов, ассоциированы с массивом leds
}

void loop() {

  // по умолчанию все диоды имеют яркость = 0 (полностью темные)
  fill_solid(&(leds[startPartA]), partLength, CHSV(color1, 255, 0));
  fill_solid(&(leds[startPartB]), partLength, CHSV(color2, 255, 0));
  fill_solid(&(leds[startPartC]), partLength, CHSV(color3, 255, 0));

  //  переключалка режимов
  if (digitalRead(modeChangePin)) {
    currentMode += 1;
    if (currentMode > 2) {
      currentMode = 0;
    }
    while (digitalRead(modeChangePin)) {}
  }

  // РЕЖИМ 0 (RED)
  if (currentMode == 0) {

    // Индикация текущего режима
    digitalWrite( bPin, LOW );
    digitalWrite( rPin, HIGH );

    // Если  сработал сенсор B
    if (digitalRead(sensorPinB)) {

      // яркость на максиум
      brightnessA = 255;
      brightnessB = 255;
      brightnessC = 255;
      FastLED.show();
      while (digitalRead(sensorPinB)) {}

      // Затемняем пока не погаснет
      while (brightnessA > 0) {
        brightnessA -= blackoutStep;
        brightnessB -= blackoutStep;
        brightnessC -= blackoutStep;
        fill_solid(&(leds[startPartA]), partLength, CHSV(color1, 255, brightnessA));
        fill_solid(&(leds[startPartB]), partLength, CHSV(color2, 255, brightnessB));
        fill_solid(&(leds[startPartC]), partLength, CHSV(color3, 255, brightnessC));
        FastLED.show();

        // если нажали во время затемнения - выход из цикла
        if (digitalRead(sensorPinB)) {
          break;
        }
      }
    }
  }

  // РЕЖИМ 1 (GREEN)
  if (currentMode == 1) {
    digitalWrite( rPin, LOW );
    digitalWrite( gPin, HIGH );
  }

  //РЕЖИМ 2 (BLUE)
  if (currentMode == 2) {
    digitalWrite( gPin, LOW );
    digitalWrite( bPin, HIGH );
  }
}
