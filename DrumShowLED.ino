//ПОДКЛЮЧЕНИЕ БИБЛИОТЕКИ
#include <FastLED.h>    //подключаем библиотеку
#define NUM_LEDS 210    // общее количество светодиодов
#define DATA_PIN 8      // пин подключенный к ленте
CRGB leds[NUM_LEDS];    // массив для манипуляции светодиодами

// ПОДКЛЮЧЕНИЯ УСТРОЙСТВ
const uint8_t pinSoundSensor_0 = 13;
const uint8_t pinSoundSensor_1 = 12;
const uint8_t pinSoundSensor_2 = 11;
const uint8_t pinColorSlider_0 = A0;
const uint8_t pinColorSlider_1 = A1;
const uint8_t pinColorSlider_2 = A2;
const uint8_t pinFadeSlider = A7;
const uint8_t pinModeChangeButton = 2;

// РАЗМЕТКА ЛЕНТЫ
const uint8_t partLength = 60;
const uint8_t startPart_0 = 30;
const uint8_t startPart_1 = 90;
const uint8_t startPart_2 = 150;

//ПЕРЕМЕННЫЕ
uint8_t currentMode = 0;
uint8_t hue[3];
uint8_t saturation[3];
uint8_t value[3];
float fadeStep;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

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

// ДАТЧИКИ ЗВУКА
//  if (digitalRead(pinSoundSensor_0) == LOW) {Serial.println ("Sound sensor 0");}
//  if (digitalRead(pinSoundSensor_1) == LOW) {Serial.println ("Sound sensor 1");}
//  if (digitalRead(pinSoundSensor_2) == LOW) {Serial.println ("Sound sensor 2");}  

  // ПЕРЕКЛЮЧЕНИЕ РЕЖИМОВ
  if (digitalRead(pinModeChangeButton)) {
    Serial.print(currentMode);
    changeMode();
    Serial.print(" mode changed to: ");
    Serial.println(currentMode);
    while (digitalRead(pinModeChangeButton)) {}
  }

  // ИЗМЕНЕНИЯ ЦВЕТА
  uint8_t colorSliderValue[3] = {transformSliderValueToColor(analogRead(pinColorSlider_0)), transformSliderValueToColor(analogRead(pinColorSlider_1)), transformSliderValueToColor(analogRead(pinColorSlider_2))};
  hue[0] = smootlyChangeCurrentColorToSettedOnSlider(hue[0],colorSliderValue[0]);
  hue[1] = smootlyChangeCurrentColorToSettedOnSlider(hue[1],colorSliderValue[1]);
  hue[2] = smootlyChangeCurrentColorToSettedOnSlider(hue[2],colorSliderValue[2]);
  saturation[0] = smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(saturation[0], hue[0]);
  saturation[1] = smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(saturation[1], hue[1]);
  saturation[2] = smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(saturation[2], hue[2]);  
   
  // FADE СЛАЙДЕР
  //  Serial.println(analogRead(pinFadeSlider));

  //ИНТЕРФЕЙС
  fill_solid(&(leds[0]), 1, CHSV(hue[0], saturation[0], 255 ));
  fill_solid(&(leds[1]), 1, CHSV(hue[1], saturation[1], 255 ));
  fill_solid(&(leds[2]), 1, CHSV(hue[2], saturation[2], 255 ));  
  FastLED.show();
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
      currentMode=0; 
      leds[4] = 0x001100; leds[5] = 0x111111; leds[6] = 0x001100;
      break;
  }
}

uint8_t transformSliderValueToColor(int value){
  uint8_t color;
       if (value == 0)    color = 0;
  else if (value < 15)    color = 32;
  else if (value < 92)    color = 64;
  else if (value < 409)   color = 96;
  else if (value < 725)   color = 128;
  else if (value < 981)   color = 160;
  else if (value < 1011)  color = 192;
  else if (value < 1022)  color = 244;
  else                    color = 254; 
                          return color;
}

uint8_t smootlyChangeCurrentColorToSettedOnSlider(uint8_t currentColor, uint8_t neededColor){
  uint8_t colorChangeStep = 2;
  if (currentColor > neededColor) currentColor -= colorChangeStep;
  else if (currentColor < neededColor) currentColor += colorChangeStep;
  return currentColor;
}

uint8_t smoothlyRemoveSaturationAtTheTopPositionOfTheColorSlider(uint8_t saturation,uint8_t hue){
  uint8_t saturationChangeStep = 5;
  if      (hue == 254 && saturation > 0)                      saturation -= saturationChangeStep;
  else if (hue == 254 && saturation <= saturationChangeStep)  saturation = 0;
  else if (saturation < 255)                                  saturation += saturationChangeStep;
  return saturation;  
}
