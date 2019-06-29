// Подключаем библиотеку
#include <FastLED.h>    //подключаем библиотеку
#define NUM_LEDS 210    // общее количество светодиодов
#define DATA_PIN 8      // пин подключенный к ленте
CRGB leds[NUM_LEDS];    // массив для манипуляции светодиодами

// Подключаем устройства
const uint8_t pinSoundSensor_0 = 13;
const uint8_t pinSoundSensor_1 = 12;
const uint8_t pinSoundSensor_2 = 11;

const uint8_t pinColorSlider_0 = A0;
const uint8_t pinColorSlider_1 = A1;
const uint8_t pinColorSlider_2 = A2;

const uint8_t pinTouchSensor_0 = 3;
const uint8_t pinTouchSensor_1 = 4;
const uint8_t pinTouchSensor_2 = 5;

const uint8_t pinFadeSlider = A7;
const uint8_t pinModeChangeButton = 2;

// Разметка ленты
const uint8_t partLength = 60;
const uint8_t startPart_0 = 30;
const uint8_t startPart_1 = 90;
const uint8_t startPart_2 = 150;

// Переменные
uint8_t currentMode = 0;
uint8_t hue[3];
uint8_t saturation[3];
float value[3];
float fadeStep;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  leds[4] = 0x001100; leds[5] = 0x111111; leds[6] = 0x001100;
  
  //  СЧИТЫВЕАЕМ ПОКАЗАНИЯ С ДАТЧИКОВ
  //  pinMode(pinSoundSensor_0, INPUT);
  //  pinMode(pinSoundSensor_1, INPUT);
  //  pinMode(pinSoundSensor_2, INPUT);
  //  pinMode(pinColorSlider_0, INPUT);
  //  pinMode(pinColorSlider_1, INPUT);
  //  pinMode(pinColorSlider_2, INPUT);
  //  pinMode(pinFadeSlider, INPUT);
  //  pinMode(pinModeChangeButton, INPUT);
}

void loop() {

  // Датчики звука (жду аналоговые)
  //  if (digitalRead(pinSoundSensor_0) == LOW) {Serial.println ("Sound sensor 0");}
  //  if (digitalRead(pinSoundSensor_1) == LOW) {Serial.println ("Sound sensor 1");}
  //  if (digitalRead(pinSoundSensor_2) == LOW) {Serial.println ("Sound sensor 2");}

  // Переключатель режимов
  if (digitalRead(pinModeChangeButton)) {
    changeMode();
    while (digitalRead(pinModeChangeButton)) {}
  }

  // Вычисляем и плавно изменяем HUE и SATURATION на основании показаний слайдера цвета
  uint8_t colorSliderValue[3] = {transformSliderValueToColor(analogRead(pinColorSlider_0)), transformSliderValueToColor(analogRead(pinColorSlider_1)), transformSliderValueToColor(analogRead(pinColorSlider_2))};
  hue[0] = smootlyChangeCurrentColorToSettedOnSlider(hue[0], colorSliderValue[0]);
  hue[1] = smootlyChangeCurrentColorToSettedOnSlider(hue[1], colorSliderValue[1]);
  hue[2] = smootlyChangeCurrentColorToSettedOnSlider(hue[2], colorSliderValue[2]);
  saturation[0] = smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(saturation[0], hue[0]);
  saturation[1] = smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(saturation[1], hue[1]);
  saturation[2] = smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(saturation[2], hue[2]);

  // При пиркосновении к датчикам касания устанавливаем VALUE на максимум в зависимости от режима
  if (digitalRead(pinTouchSensor_0)) {
    if (currentMode == 1) {value[0] = 255; value[2] = 255;}
    else if (currentMode == 2) {value[0] = 255;}
  }

  if (digitalRead(pinTouchSensor_1)) {
    if (currentMode == 0) {value[0] = 255; value[1] = 255; value[2] = 255;}
    else value[1] = 255;
  }

  if (digitalRead(pinTouchSensor_2) && currentMode == 2) {
    value[2] = 255;
  }

  // Подсвечиваем ленты вычисленными HSV
  fill_solid(&(leds[startPart_0]), partLength, CHSV(hue[0], saturation[0], value[0]));
  fill_solid(&(leds[startPart_1]), partLength, CHSV(hue[1], saturation[1], value[1]));
  fill_solid(&(leds[startPart_2]), partLength, CHSV(hue[2], saturation[2], value[2]));
  FastLED.show();

  // Вычисляем скорость затемнения
  fadeStep = setFadeStepFromFadeSliderValue(analogRead(pinFadeSlider));
  
  // Затемняем
    for (uint8_t i = 0; i < 3; i++){
      value[i] -= fadeStep;
      if (value[i] == 0 || value[i] < fadeStep) value[i] = 0;
    } 

  //ИНТЕРФЕЙС
  fill_solid(&(leds[0]), 1, CHSV(hue[0], saturation[0], 255 ));
  fill_solid(&(leds[1]), 1, CHSV(hue[1], saturation[1], 255 ));
  fill_solid(&(leds[2]), 1, CHSV(hue[2], saturation[2], 255 ));
}

// ФУНКЦИИ
void changeMode() {
  switch (currentMode) {
    case 0:
      currentMode++;
      leds[4] = 0x111111; leds[5] = 0x111111; leds[6] = 0x001100;
      break;
    case 1:
      currentMode++;
      leds[4] = 0x111111; leds[5] = 0x111111; leds[6] = 0x111111;
      break;
    case 2:
      currentMode = 0;
      leds[4] = 0x001100; leds[5] = 0x111111; leds[6] = 0x001100;
      break;
  }     
}

uint8_t transformSliderValueToColor(int sliderValue) {
  uint8_t color;
  if (sliderValue == 0)    color = 0;
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

float setFadeStepFromFadeSliderValue(int sliderValue){
  float fadeStep;
  uint8_t indicator;
  
       if (sliderValue < 15)    { fadeStep = 127.5; indicator = 0; }
  else if (sliderValue < 92)    { fadeStep = 42.5; indicator = 1; }
  else if (sliderValue < 409)   { fadeStep = 25; indicator = 2; }
  else if (sliderValue < 725)   { fadeStep = 15; indicator = 3; }
  else if (sliderValue < 981)   { fadeStep = 6; indicator = 4;}
  else if (sliderValue < 1011)  { fadeStep = 3; indicator = 5;}
  else if (sliderValue < 1022)  { fadeStep = 1; indicator = 6;}
  
  fill_solid(&(leds[8 + indicator]), (6 - indicator), CHSV(0, 255, 100 ));
  fill_solid(&(leds[8]), indicator, CHSV(100, 0, 100 ));

  Serial.println(indicator);
  return fadeStep;
}
