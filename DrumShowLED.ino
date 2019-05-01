#include <FastLED.h> //подключаем библиотеку
#define NUM_LEDS 60  // общее количество светодиодов
#define DATA_PIN 8   // пин подключенный к ленте
CRGB leds[NUM_LEDS]; // массив для манипуляции светодиодами

const int sensorPinA = 3; // Куда подключен сенсор А
const int sensorPinB = 4; // Куда подключен сенсор B
const int sensorPinC = 5; // Куда подключен сенсор C

int colorA = 96;  // дефолтный цвет отрезка 1
int colorB = 0;   // дефолтный цвет отрезка 2
int colorC = 160; // дефолтный цвет отрезка 3

float brightnessA = 0;    // дефолтная яркость A
float brightnessB = 0;    // дефолтная яркость B
float brightnessC = 0;    // дефолтная яркость C

float blackoutStep = 3; //доступные шаги затемнения: 0.5, 1, 3, 5, 15, 17, 42.5

const int modeChangePin = 2; // Куда подключена кнопка смены режимов

int currentMode = 0;  // переменная для переключения режимов

const byte rPin = 13; // Пины индикатора переключения режимов
const byte gPin = 12; // Пины индикатора переключения режимов
const byte bPin = 11; // Пины индикатора переключения режимов

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

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // декларируем библиотеке с чем ей работать
}

void loop() {

  // Плавно затемняем все светодиоды у которых не нулевая яркость
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
  fill_solid(&(leds[startPartA]), partLength, CHSV(colorA, 255, brightnessA ));
  fill_solid(&(leds[startPartB]), partLength, CHSV(colorB, 255, brightnessB ));
  fill_solid(&(leds[startPartC]), partLength, CHSV(colorC, 255, brightnessC ));
  FastLED.show();


  //  ПЕРЕКЛЮЧАЛКА РЕЖИМОВ
  if (digitalRead(modeChangePin)) {
    currentMode += 1;
    if (currentMode > 2) {
      currentMode = 0;
    }
    while (digitalRead(modeChangePin)) {
      colorA++;
      colorB++;
      colorC++;
    }
  }

  // РЕЖИМ 0 (RED)
  if (currentMode == 0) {
    digitalWrite( bPin, LOW ); digitalWrite( rPin, HIGH );

    if (digitalRead(sensorPinB)) {
      brightnessA = 255;
      brightnessB = 255;
      brightnessC = 255;
    }
  }
  // РЕЖИМ 1 (GREEN)
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
  //РЕЖИМ 2 (BLUE)
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
}