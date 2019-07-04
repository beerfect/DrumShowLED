#include <FastLED.h>    // Подключаем библиотеку для управления LED лентой
#define NUM_LEDS 210    // Общее число светодиодов
#define DATA_PIN 8      // Пин подключения LED ленты
CRGB leds[NUM_LEDS];    // Массив для управления LED лентой

// Подключаем устройства
const uint8_t pinTouchSensor[3] = {3, 4, 5};    // Пины подключения сенсорных кнопок
const uint8_t pinSoundSensor[3] = {A4, A5, A6}; // Пины подключения датчиков звука
const uint8_t pinColorSlider[3] = {A0, A1, A2}; // Пины подключения слйдеров изменения HUE
const uint8_t pinFadeSlider = A3;               // Пин подключения слайдера затемнения
const uint8_t pinModeChangeButton = 2;          // Пин подключения кнопки пепеключения режимов

// Разметка ленты
const uint8_t partLength = 60;              // Количество светодиодов у каждого барабана
const uint8_t startPart[3] = {30, 90, 150}; // Начала отрезков ленты

// Переменные
uint8_t currentMode = 0;        // Текущий режим
uint8_t currentHue[3];          // Текущие Hue
uint8_t neededHue[3];           // Hue к которым нужно придти
uint8_t ledsSaturation[3];      // Saturation's для лент
uint8_t interfaceSaturation[3]; // Saturation's для интерфейса
float value[3];                 // Value's
float fadeStep;                 // Скорость затемнения после удара

void setup() {
  Serial.begin(9600);                                         // Подключаем монитор порта для отладки
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);        // Доподключаем библиотеку FastLED
  leds[4] = 0x001100; leds[5] = 0x111111; leds[6] = 0x001100; // Отображаем в интерфейсе режим по умолчанию
}

void loop() {
  // Переключатель режимов
  if (digitalRead(pinModeChangeButton)) {
    changeMode();
    while (digitalRead(pinModeChangeButton)) {}
  }

  // Вычисляем и плавно изменяем HUE и SATURATION на основании показаний слайдера цвета
  for (uint8_t i = 0; i < 3; i++) {
    neededHue[i] = getHue(analogRead(pinColorSlider[i]));
    currentHue[i] = smoothlyChangeHue(currentHue[i], neededHue[i]);
    ledsSaturation[i] = smoothlyChangeSaturation(ledsSaturation[i], currentHue[i]);
  }

  // Слушаем датчики, применяем режимы
  if (digitalRead(pinTouchSensor[0])) {
    if (currentMode == 1) {
      value[0] = 255;
      value[2] = 255;
    }
    else if (currentMode == 2) {
      value[0] = 255;
    }
  }

  if (digitalRead(pinTouchSensor[1])) {
    if (currentMode == 0) {
      value[0] = 255;
      value[1] = 255;
      value[2] = 255;
    }
    else value[1] = 255;
  }

  if (digitalRead(pinTouchSensor[2]) && currentMode == 2) {
    value[2] = 255;
  }

  // Подсвечиваем ленты вычисленными HSV
  for (uint8_t i = 0; i < 3; i++) {
    fill_solid(&(leds[startPart[i]]), partLength, CHSV(currentHue[i], ledsSaturation[i], value[i]));
  }  
  FastLED.show();
  
  // Вычисляем скорость затемнения
  fadeStep = setFadeStepFromFadeSliderValue(analogRead(pinFadeSlider));

  // Затемняем
  for (uint8_t i = 0; i < 3; i++) {
    value[i] -= fadeStep;
    if (value[i] == 0 || value[i] < fadeStep) value[i] = 0;
  }

  // Интерфейс
  for (uint8_t i = 0; i < 3; i++) {
    if(analogRead(pinColorSlider[i]) > 1021) interfaceSaturation[i] = 0;
    else interfaceSaturation[i] = 255;
    fill_solid(&(leds[i]), 1, CHSV(neededHue[i], interfaceSaturation[i], 255 ));
  }  
}

// ФУНКЦИИ

// Циклично переключает режимы (0/1/2), отображает индикацию текущего режима 
void changeMode() {
  switch (currentMode) {
    case 0:
      currentMode++;
      leds[4] = 0x111111; 
      leds[5] = 0x111111; 
      leds[6] = 0x001100;
      break;
    case 1:
      currentMode++;
      leds[4] = 0x111111; 
      leds[5] = 0x111111; 
      leds[6] = 0x111111;
      break;
    case 2:
      currentMode = 0;
      leds[4] = 0x001100; 
      leds[5] = 0x111111; 
      leds[6] = 0x001100;
      break;
  }
}

// Диапазон хода слайдера разбит на равные отрезки, за каждым отрезком закреплен свой цвет
uint8_t getHue(int sliderValue) {
   if      (sliderValue == 0)    return 0;    // Красный (нижнее положение слайдера)
  else if (sliderValue < 15)    return 32;   // Оранжевый
  else if (sliderValue < 92)    return 64;   // Желтый
  else if (sliderValue < 409)   return 96;   // Зеленый
  else if (sliderValue < 725)   return 128;  // Голубой
  else if (sliderValue < 981)   return 160;  // Синий
  else if (sliderValue < 1011)  return 192;  // Фиолетовый
  else if (sliderValue < 1022)  return 244;  // Розовый
  else                          return 254;  // Красный (верхнее положение слайдера)
}

// Плавно изменяем текущий цвет на цвет, установленый на слайдере
uint8_t smoothlyChangeHue(uint8_t currentColor, uint8_t neededColor) {
  uint8_t colorChangeStep = 1;
  if (currentColor > neededColor) currentColor -= colorChangeStep;
  else if (currentColor < neededColor) currentColor += colorChangeStep;
  return currentColor;
}

// В верхнем положении слайдера цвета плавно включаем белый цвет
uint8_t smoothlyChangeSaturation(uint8_t saturation, uint8_t hue) {
  uint8_t saturationChangeStep = 5;
  if      (hue == 254 && saturation > 0)                      saturation -= saturationChangeStep;
  else if (hue == 254 && saturation <= saturationChangeStep)  saturation = 0;
  else if (saturation < 255)                                  saturation += saturationChangeStep;
  return saturation;
}

// Регулируем слайдером скорость затемнения после удара, отображаем индикацию
float setFadeStepFromFadeSliderValue(int sliderValue) {
  float fadeStep;
  uint8_t indicator;

  if      (sliderValue < 15)    {fadeStep = 127.5; indicator = 0;}
  else if (sliderValue < 92)    {fadeStep = 15;    indicator = 1;}
  else if (sliderValue < 409)   {fadeStep = 6;     indicator = 2;}
  else if (sliderValue < 725)   {fadeStep = 4;     indicator = 3;}
  else if (sliderValue < 981)   {fadeStep = 3;     indicator = 4;}
  else if (sliderValue < 1011)  {fadeStep = 1.5;   indicator = 5;}
  else if (sliderValue < 1022)  {fadeStep = 1;     indicator = 6;}

  // Интерфейс
  fill_solid(&(leds[8 + indicator]), (6 - indicator), CHSV(0, 255, 100 ));
  fill_solid(&(leds[8]), indicator, CHSV(100, 0, 100 ));

  Serial.println(indicator);
  return fadeStep;
}
