//ПОДКЛЮЧЕНИЕ БИБЛИОТЕКИ
#include <FastLED.h>    //подключаем библиотеку
#define NUM_LEDS 210    // общее количество светодиодов
#define DATA_PIN 8      // пин подключенный к ленте
CRGB leds[NUM_LEDS];    // массив для манипуляции светодиодами

// ПОДКЛЮЧЕНИЕ СЛАЙДЕРОВ
int sliderPinA = A0;        // Куда подключен слайдер А
int sliderPinB = A1;        // Куда подключен слайдер B
int sliderPinC = A2;        // Куда подключен слайдер C
int sliderPinBlackout = A7; // Куда подключен слайдер регулировки длительности затухания

// ПОДКЛЮЧЕНИЕ ДАТЧИКОВ
const int sensorPinA = 3; // Куда подключен датчик звука А
const int sensorPinB = 4; // Куда подключен датчик звука B
const int sensorPinC = 5; // Куда подключен датчик звука C

// ЦВЕТ, ЯРКОСТЬ, ЗАТЕМНЕНИЕ
int colorA = 0;             // дефолтный цвет отрезка 1
int colorB = 0;             // дефолтный цвет отрезка 2
int colorC = 0;             // дефолтный цвет отрезка 3
float brightnessA = 0;      // дефолтная яркость A
float brightnessB = 0;      // дефолтная яркость B
float brightnessC = 0;      // дефолтная яркость C
int saturationA = 255;      // дефолтная насыщенность ленты А
int saturationB = 255;      // дефолтная насыщенность ленты B
int saturationC = 255;      // дефолтная насыщенность ленты C

float blackoutStep = 0.5;     //доступные шаги затемнения: 0.5, 1.5, 1, 3, 5, 15, 17, 42.5

//ПЕРЕКЛЮЧЕНИЕ РЕЖИМОВ
const int modeChangePin = 2; // Куда подключена кнопка смены режимов
int currentMode = 0;         // переменная для переключения режимов

//РАСЧЕТЫ
int partLength = 60;        // количество светодиодов у каждого барабана
int startPartA = 30;        // начало первого отрезка
int startPartB = 90;        // начало второго отрезка
int startPartC = 150;       // начало третьего отрезка

void setup() {
  // ПРОВЕРКА
    Serial.begin(9600);

  // ДЕКЛАРИРУЕМ БИБЛИОТЕКЕ FASTLED С ЧЕМ ЕЙ РАБОТАТЬ
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // ДАТЧИКИ
  pinMode(sensorPinA, INPUT);
  pinMode(sensorPinB, INPUT);
  pinMode(sensorPinC, INPUT);
}

void loop() {
  
  // СКОРОСТЬ ЗАТЕМНЕНИЯ - ЛОГИКА И ИНДИКАЦИЯ
  int blackoutMode;
  int sliderValueBlackout = analogRead(sliderPinBlackout);
       if (sliderValueBlackout < 8)    {blackoutMode = 0;}
  else if (sliderValueBlackout < 31)   {blackoutMode = 1;}
  else if (sliderValueBlackout < 179)  {blackoutMode = 2;}
  else if (sliderValueBlackout < 464)  {blackoutMode = 3;}
  else if (sliderValueBlackout < 736)  {blackoutMode = 4;}
  else if (sliderValueBlackout < 980)  {blackoutMode = 5;}
  else if (sliderValueBlackout < 1011) {blackoutMode = 6;}
  else if (sliderValueBlackout < 1024) {blackoutMode = 7;}
       if (blackoutMode == 7) {blackoutStep = 1.5;   leds[6] = 0x111111; leds[7] = 0x111111; leds[8] = 0x111111; leds[9] = 0x111111; leds[10] = 0x111111; leds[11] = 0x111111; leds[12] = 0x111111;}
  else if (blackoutMode == 6) {blackoutStep = 3;     leds[6] = 0x111111; leds[7] = 0x111111; leds[8] = 0x111111; leds[9] = 0x111111; leds[10] = 0x111111; leds[11] = 0x111111; leds[12] = 0x110000;}               
  else if (blackoutMode == 5) {blackoutStep = 6;     leds[6] = 0x111111; leds[7] = 0x111111; leds[8] = 0x111111; leds[9] = 0x111111; leds[10] = 0x111111; leds[11] = 0x110000; leds[12] = 0x110000;}
  else if (blackoutMode == 4) {blackoutStep = 15;    leds[6] = 0x111111; leds[7] = 0x111111; leds[8] = 0x111111; leds[9] = 0x111111; leds[10] = 0x110000; leds[11] = 0x110000; leds[12] = 0x110000;}
  else if (blackoutMode == 3) {blackoutStep = 25;    leds[6] = 0x111111; leds[7] = 0x111111; leds[8] = 0x111111; leds[9] = 0x110000; leds[10] = 0x110000; leds[11] = 0x110000; leds[12] = 0x110000;}
  else if (blackoutMode == 2) {blackoutStep = 42.5;  leds[6] = 0x111111; leds[7] = 0x111111; leds[8] = 0x110000; leds[9] = 0x110000; leds[10] = 0x110000; leds[11] = 0x110000; leds[12] = 0x110000;}
  else if (blackoutMode == 1) {blackoutStep = 63.75; leds[6] = 0x111111; leds[7] = 0x110000; leds[8] = 0x110000; leds[9] = 0x110000; leds[10] = 0x110000; leds[11] = 0x110000; leds[12] = 0x110000;}
  else if (blackoutMode == 0) {blackoutStep = 127.5; leds[6] = 0x110000; leds[7] = 0x110000; leds[8] = 0x110000; leds[9] = 0x110000; leds[10] = 0x110000; leds[11] = 0x110000; leds[12] = 0x110000;}

  //  РАВНОМЕРНОЕ РАСПРЕДЕЛЕНИЕ RAINBOW СПЕКТРА ПО ХОДУ СЛАЙДЕРА
  int sliderValueA = analogRead(sliderPinA);  // получаем текущее значение
  int sliderValueB = analogRead(sliderPinB);  // получаем текущее значение
  int sliderValueC = analogRead(sliderPinC);  // получаем текущее значение
  
  int colorChangeStep = 4;

       if (sliderValueA < 8)        {colorA -= colorChangeStep;if (colorA < 0) {colorA = 0;}}
  else if (sliderValueA < 31)   {if (colorA < 32)  {colorA += colorChangeStep;}else if (colorA > 32)  {colorA -= colorChangeStep;}}
  else if (sliderValueA < 179)  {if (colorA < 64)  {colorA += colorChangeStep;}else if (colorA > 64)  {colorA -= colorChangeStep;}}
  else if (sliderValueA < 464)  {if (colorA < 96)  {colorA += colorChangeStep;}else if (colorA > 96)  {colorA -= colorChangeStep;}}
  else if (sliderValueA < 736)  {if (colorA < 128) {colorA += colorChangeStep;}else if (colorA > 128) {colorA -= colorChangeStep;}} 
  else if (sliderValueA < 980)  {if (colorA < 160) {colorA += colorChangeStep;}else if (colorA > 160) {colorA -= colorChangeStep;}}
  else if (sliderValueA < 1011) {if (colorA < 192) {colorA += colorChangeStep;}else if (colorA > 192) {colorA -= colorChangeStep;}}
  else if (sliderValueA < 1023) {if (colorA < 244) {colorA += colorChangeStep;}else if (colorA > 244) {colorA -= colorChangeStep;}}
  else if (sliderValueA == 1023)    {colorA = 255; }  
  
       if (sliderValueB < 8)        {colorB -= colorChangeStep;if (colorB < 0) {colorB = 0;}}
  else if (sliderValueB < 31)   {if (colorB < 32)  {colorB += colorChangeStep;}else if (colorB > 32)  {colorB -= colorChangeStep;}}
  else if (sliderValueB < 179)  {if (colorB < 64)  {colorB += colorChangeStep;}else if (colorB > 64)  {colorB -= colorChangeStep;}}
  else if (sliderValueB < 464)  {if (colorB < 96)  {colorB += colorChangeStep;}else if (colorB > 96)  {colorB -= colorChangeStep;}}
  else if (sliderValueB < 736)  {if (colorB < 128) {colorB += colorChangeStep;}else if (colorB > 128) {colorB -= colorChangeStep;}} 
  else if (sliderValueB < 980)  {if (colorB < 160) {colorB += colorChangeStep;}else if (colorB > 160) {colorB -= colorChangeStep;}}
  else if (sliderValueB < 1011) {if (colorB < 192) {colorB += colorChangeStep;}else if (colorB > 192) {colorB -= colorChangeStep;}}
  else if (sliderValueB < 1023) {if (colorB < 244) {colorB += colorChangeStep;}else if (colorB > 244) {colorB -= colorChangeStep;}}
  else if (sliderValueB == 1023){colorB = 255;}

       if (sliderValueC < 8)        {colorC -= colorChangeStep;if (colorC < 0) {colorC = 0;}}
  else if (sliderValueC < 31)   {if (colorC < 32)  {colorC += colorChangeStep;}else if (colorC > 32)  {colorC -= colorChangeStep;}}
  else if (sliderValueC < 179)  {if (colorC < 64)  {colorC += colorChangeStep;}else if (colorC > 64)  {colorC -= colorChangeStep;}}
  else if (sliderValueC < 464)  {if (colorC < 96)  {colorC += colorChangeStep;}else if (colorC > 96)  {colorC -= colorChangeStep;}}
  else if (sliderValueC < 736)  {if (colorC < 128) {colorC += colorChangeStep;}else if (colorC > 128) {colorC -= colorChangeStep;}} 
  else if (sliderValueC < 980)  {if (colorC < 160) {colorC += colorChangeStep;}else if (colorC > 160) {colorC -= colorChangeStep;}}
  else if (sliderValueC < 1011) {if (colorC < 192) {colorC += colorChangeStep;}else if (colorC > 192) {colorC -= colorChangeStep;}}
  else if (sliderValueC < 1023) {if (colorC < 244) {colorC += colorChangeStep;}else if (colorC > 244) {colorC -= colorChangeStep;}}
  else if (sliderValueC == 1023)    {colorC = 255;}

  //  ПЛАВНОЕ ПОЯВЛЕНИЕ БЕЛОГО ЦВЕТА В ВЕРХНЕМ ПОЛОЖЕНИИ СЛАЙДЕРА
  int whiteColorShowSpeed = 30;  
  if (colorA == 255) {saturationA -= whiteColorShowSpeed;if (saturationA < 0) {saturationA = 0;}
  } else {saturationA += whiteColorShowSpeed;if (saturationA > 255) {saturationA = 255;}}
  if (colorB == 255) {saturationB -= whiteColorShowSpeed;if (saturationB < 0) {saturationB = 0;}
  } else {saturationB += whiteColorShowSpeed;if (saturationB > 255) {saturationB = 255;}}
  if (colorC == 255) {saturationC -= whiteColorShowSpeed;if (saturationC < 0) {saturationC = 0;}
  } else {saturationC += whiteColorShowSpeed;if (saturationC > 255) {saturationC = 255;}}

  // ТРИ СВЕТОДИОДА ПОКАЗЫВАЮТ ТЕКУЩИЕ ЦВЕТА ЛЕНТ
  fill_solid(&(leds[0]), 1, CHSV(colorA, saturationA, 255 ));
  fill_solid(&(leds[1]), 1, CHSV(colorB, saturationB, 255 ));
  fill_solid(&(leds[2]), 1, CHSV(colorC, saturationC, 255 ));
  FastLED.show();

  // ЗАТЕМНЯЕМ ВСЕ ЛЕНТЫ У КОТОРЫХ НЕНУЛЕВАЯ ЯРКОСТЬ
  brightnessA -= blackoutStep; if (brightnessA < 0) {brightnessA = 0;}
  brightnessB -= blackoutStep; if (brightnessB < 0) {brightnessB = 0;}
  brightnessC -= blackoutStep; if (brightnessC < 0) {brightnessC = 0;}

  // ЗАЖИГАЕМ ЛЕНТЫ
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

    // Отображаем индикацию
    leds[3] = 0x001100;
    leds[4] = 0x111111;
    leds[5] = 0x001100;
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

    // Отображаем индикацию
    leds[3] = 0x111111;
    leds[4] = 0x111111;
    leds[5] = 0x001100;
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

    // Отображаем индикацию
    leds[3] = 0x111111;
    leds[4] = 0x111111;
    leds[5] = 0x111111;
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
