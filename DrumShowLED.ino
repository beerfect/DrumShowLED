// Подключаем библиотеку
#include <FastLED.h>
#define NUM_LEDS 210
#define DATA_PIN 8
CRGB leds[NUM_LEDS];

// Подключаем устройства
const uint8_t pinTouchSensor[3] = {3, 4, 5};
const uint8_t pinSoundSensor[3] = {A4, A5, A6};
const uint8_t pinColorSlider[3] = {A0, A1, A2};
const uint8_t pinFadeSlider = A3;
const uint8_t pinModeChangeButton = 2;

// Разметка ленты
const uint8_t partLength = 60;
const uint8_t startPart[3] = {30, 90, 150};

// Переменные
uint8_t currentMode = 0;
uint8_t colorSliderValue[3];
uint8_t hue[3];
uint8_t saturation[3];
float value[3];
float fadeStep;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  leds[4] = 0x001100; leds[5] = 0x111111; leds[6] = 0x001100;
}

void loop() {
  // Переключатель режимов
  if (digitalRead(pinModeChangeButton)) {
    changeMode();
    while (digitalRead(pinModeChangeButton)) {}
  }

  // Вычисляем и плавно изменяем HUE и SATURATION на основании показаний слайдера цвета
  for (uint8_t i = 0; i < 3; i++) {
    colorSliderValue[i] = transformSliderValueToColor(analogRead(pinColorSlider[i]));
    hue[i] = smootlyChangeCurrentColorToSettedOnSlider(hue[i], colorSliderValue[i]);
    saturation[i] = smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(saturation[i], hue[i]);
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
    fill_solid(&(leds[startPart[i]]), partLength, CHSV(hue[i], saturation[i], value[i]));
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
    fill_solid(&(leds[i]), 1, CHSV(hue[i], saturation[i], 255 ));
  }

  
}

// ФУНКЦИИ
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

uint8_t transformSliderValueToColor(int sliderValue) {
  uint8_t color;
  if      (sliderValue == 0)    color = 0;
  else if (sliderValue < 15)    color = 32;
  else if (sliderValue < 92)    color = 64;
  else if (sliderValue < 409)   color = 96;
  else if (sliderValue < 725)   color = 128;
  else if (sliderValue < 981)   color = 160;
  else if (sliderValue < 1011)  color = 192;
  else if (sliderValue < 1022)  color = 244;
  else                          color = 254;
  return color;
}

uint8_t smootlyChangeCurrentColorToSettedOnSlider(uint8_t currentColor, uint8_t neededColor) {
  uint8_t colorChangeStep = 2;
  if (currentColor > neededColor) currentColor -= colorChangeStep;
  else if (currentColor < neededColor) currentColor += colorChangeStep;
  return currentColor;
}

uint8_t smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(uint8_t saturation, uint8_t hue) {
  uint8_t saturationChangeStep = 5;
  if      (hue == 254 && saturation > 0)                      saturation -= saturationChangeStep;
  else if (hue == 254 && saturation <= saturationChangeStep)  saturation = 0;
  else if (saturation < 255)                                  saturation += saturationChangeStep;
  return saturation;
}

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
