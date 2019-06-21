//ПОДКЛЮЧЕНИЕ БИБЛИОТЕКИ
#include <FastLED.h>    //подключаем библиотеку
#define NUM_LEDS 210  // общее количество светодиодов
#define DATA_PIN 8      // пин подключенный к ленте
CRGB leds[NUM_LEDS];    // массив для манипуляции светодиодами

// ПОДКЛЮЧЕНИЕ СЛАЙДЕРОВ
int sliderPinA = A0; // Куда подключен слайдер А
int sliderPinB = A1; // Куда подключен слайдер B
int sliderPinC = A2; // Куда подключен слайдер C

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
float blackoutStep = 5;   //доступные шаги затемнения: 0.5, 1.5, 1, 3, 5, 15, 17, 42.5

int saturationA = 255;       // насыщенность ленты А
int saturationB = 255;       // насыщенность ленты B
int saturationC = 255;       // насыщенность ленты C

//ПЕРЕКЛЮЧЕНИЕ РЕЖИМОВ
const int modeChangePin = 2; // Куда подключена кнопка смены режимов
int currentMode = 0;         // переменная для переключения режимов

//РАСЧЕТЫ
int partLength = 60;        // количество светодиодов у каждого барабана
int startPartA = 30;         // начало первого отрезка
int startPartB = 90;        // начало второго отрезка
int startPartC = 150;       // начало третьего отрезка

void setup() {
  // СЛАЙДЕРЫ ПРОВЕРКА
  Serial.begin(9600);

  // ДЕКЛАРИРУЕМ БИБЛИОТЕКЕ С ЧЕМ ЕЙ РАБОТАТЬ
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // ДАТЧИКИ
  pinMode(sensorPinA, INPUT);
  pinMode(sensorPinB, INPUT);
  pinMode(sensorPinC, INPUT);
}

void loop() {

  //  ПРОВЕРКА СЛАЙДЕРА
  int sliderValueA = analogRead(sliderPinA);  // получаем текущее значение
  int sliderValueB = analogRead(sliderPinB);  // получаем текущее значение
  int sliderValueC = analogRead(sliderPinC);  // получаем текущее значение

  colorA = map(sliderValueA, 0, 1023, 0, 255);
  colorB = map(sliderValueB, 0, 1023, 0, 255);
  colorC = map(sliderValueC, 0, 1023, 0, 255);

  //  Плавное появление белого цвета светодиодов в верхнем положении слайдера
  if (colorA == 255) {
    saturationA -= 15;
    if (saturationA < 0) {
      saturationA = 0;
    }
  } else {
    saturationA += 15;
    if (saturationA > 255) {
      saturationA = 255;
    }
  }

  if (colorB == 255) {
    saturationB -= 15;
    if (saturationB < 0) {
      saturationB = 0;
    }
  } else {
    saturationB += 15;
    if (saturationB > 255) {
      saturationB = 255;
    }
  }

  if (colorC == 255) {
    saturationC -= 15;
    if (saturationC < 0) {
      saturationC = 0;
    }
  } else {
    saturationC += 15;
    if (saturationC > 255) {
      saturationC = 255;
    }
  }

  // Индикация текущих цветов лент
  fill_solid(&(leds[0]), 1, CHSV(colorA, saturationA, 255 ));
  fill_solid(&(leds[1]), 1, CHSV(colorB, saturationB, 255 ));
  fill_solid(&(leds[2]), 1, CHSV(colorC, saturationC, 255 ));
  FastLED.show();

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

  //Зажигаем светодиоды
  fill_solid(&(leds[startPartA]), partLength, CHSV(colorA, saturationA, brightnessA ));
  fill_solid(&(leds[startPartB]), partLength, CHSV(colorB, saturationB, brightnessB ));
  fill_solid(&(leds[startPartC]), partLength, CHSV(colorC, saturationC, brightnessC ));
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

  // Режим "Один за всех"
  if (currentMode == 0) {

    // Отображаем индикацию текущего режима
    leds[3] = CRGB::Black;
    leds[4] = 0x111111;
    leds[5] = CRGB::Black;
    FastLED.show();

    // Подсвечиваем светодиоды
    if (digitalRead(sensorPinB)) {
      brightnessA = 255;
      brightnessB = 255;
      brightnessC = 255;
    }
  }

  // Режим "Крайние vs центральный"
  if (currentMode == 1) {

    // Отображаем индикацию текущего режима
    leds[3] = 0x111111; leds[4] = 0x111111; leds[5] = CRGB::Black;
    FastLED.show();

    // Подсвечиваем светодиоды
    if (digitalRead(sensorPinA)) {
      brightnessA = 255;
      brightnessC = 255;
    }
    if (digitalRead(sensorPinB)) {
      brightnessB = 255;
    }
  }

  //Режим "Каждый сам за себя"
  if (currentMode == 2) {

    // Отображаем индикацию текущего режима
    leds[3] = 0x111111; leds[4] = 0x111111; leds[5] = 0x111111;
    FastLED.show();

    // Подсвечиваем светодиоды
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