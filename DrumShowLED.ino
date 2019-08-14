#include <FastLED.h>    // Подключаем библиотеку для управления LED лентой
#define NUM_LEDS 210    // Общее число светодиодов
#define DATA_PIN 8      // Пин подключения LED ленты
CRGB leds[NUM_LEDS];    // Массив для управления LED лентой

const uint8_t qtyDrums = 3;

// Подключаем устройства
const uint8_t pinOnOffButton[qtyDrums] = {3, 4, 5};    // Пины подключения сенсорных кнопок
const uint8_t pinSoundSensor[qtyDrums] = {A4, A5, A6}; // Пины подключения датчиков звука

const uint8_t pinColorSlider[qtyDrums] = {A0, A1, A2}; // Пины подключения слйдеров изменения Цвет
const uint8_t pinFadeSlider = A3;                      // Пин подключения слайдера затемнения
const uint8_t pinModeChangeButton = 2;                 // Пин подключения кнопки пепеключения режимов

// Разметка ленты
const uint8_t partLength = 60;                         // Количество светодиодов у каждого барабана
const uint8_t startPart[qtyDrums] = {30, 90, 150};     // Начала отрезков ленты

// Переменные
uint8_t currentMode = 0;                             // Текущий режим
uint8_t currentColor[qtyDrums];                      // Текущие Цвет
uint8_t neededColor[qtyDrums];                       // Цвет к которому нужно придти
uint8_t ledsSaturation[qtyDrums];                    // Saturation's для лент
uint8_t interfaceSaturation[qtyDrums];               // Saturation's для интерфейса
uint8_t interfaceValue[qtyDrums] = {255, 255, 255};
float value[qtyDrums];                               // Value's
float fadeStep;                                      // Скорость затемнения после удара
uint8_t sensorThreshold[qtyDrums] = {100, 100, 100}; // порог срабатыавния пьезодатчиков
bool drumIsActive[3] = {true, true, true};           // Вкл\Выкл сенсоров

void setup() {
  Serial.begin(9600);                                         // Подключаем монитор порта для отладки
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);        // Доподключаем библиотеку FastLED
  leds[4] = 0x001100; leds[5] = 0x111111; leds[6] = 0x001100; // Отображаем в интерфейсе режим по умолчанию
}

void loop() {

  // On/Off датчички
  
   for (uint8_t i = 0; i < qtyDrums; i++) {
    if (digitalRead(pinOnOffButton[i])){
      drumIsActive[i] = !drumIsActive[i];
      changeInterface(i);
    }
    while(digitalRead(pinOnOffButton[i])){
    
    }
   }

  
  
  // Переключатель режимов
  if (digitalRead(pinModeChangeButton)) {
    changeMode();
    while (digitalRead(pinModeChangeButton)) {}
  }

  // Вычисляем и плавно изменяем Цвет и SATURATION на основании показаний слайдера цвета
  for (uint8_t i = 0; i < qtyDrums; i++) {
    neededColor[i] = getHue(analogRead(pinColorSlider[i]));
    currentColor[i] = smoothlyChangeColor(currentColor[i], neededColor[i]);
    ledsSaturation[i] = smoothlyChangeSaturation(ledsSaturation[i], currentColor[i]);
  }

  // Слушаем датчики, применяем режимы
  if (currentMode == 0){
    if (drumWasKicked(0)){
      doNothing();
    }
    if (drumWasKicked(1)){
      if (drumIsActive[0]) value[0] = 255;
      if (drumIsActive[1]) value[1] = 255;
      if (drumIsActive[2]) value[2] = 255;
    }
    if (drumWasKicked(2)){
      doNothing();
    }
  }
  
  if (currentMode == 1){
    if (drumWasKicked(0)){
      if (drumIsActive[0]) value[0] = 255;
      if (drumIsActive[2]) value[2] = 255;
    }
    if (drumWasKicked(1)){      
      if (drumIsActive[1]) value[1] = 255;      
    }
    if (drumWasKicked(2)){
      doNothing();
    }
  }
  
  if (currentMode == 2){
    if (drumWasKicked(0)){
      if (drumIsActive[0]) value[0] = 255;
    }
    if (drumWasKicked(1)){      
      if (drumIsActive[1]) value[1] = 255;      
    }
    if (drumWasKicked(2)){
      if (drumIsActive[2]) value[2] = 255;
    }
  }

  // Подсвечиваем ленты вычисленными HSV
  for (uint8_t i = 0; i < qtyDrums; i++) {
    fill_solid(&(leds[startPart[i]]), partLength, CHSV(currentColor[i], ledsSaturation[i], value[i]));
  }  
  FastLED.show();
  
  // Вычисляем скорость затемнения
  fadeStep = setFadeStepFromFadeSliderValue(analogRead(pinFadeSlider));

  // Затемняем
  for (uint8_t i = 0; i < qtyDrums; i++) {
    value[i] -= fadeStep;
    if (value[i] == 0 || value[i] < fadeStep) value[i] = 0;
  }

  // Интерфейс
  for (uint8_t i = 0; i < qtyDrums; i++) {
    if(analogRead(pinColorSlider[i]) > 1021) interfaceSaturation[i] = 0;
    else interfaceSaturation[i] = 255;
    fill_solid(&(leds[i]), 1, CHSV(neededColor[i], interfaceSaturation[i], interfaceValue[i] ));
  }  
}







// ФУНКЦИИ

void doNothing(){};

void changeInterface(uint8_t i){
  if(interfaceValue[i] == 255) {
    interfaceValue[i] = 60;  
  } else {
    interfaceValue[i] = 255; 
  }
}

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
   if      (sliderValue == 0)   return 0;    // Красный (нижнее положение слайдера)
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
uint8_t smoothlyChangeColor(uint8_t currentColor, uint8_t neededColor) {
  uint8_t colorChangeStep = 2;
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

  //  Serial.println(indicator);
  return fadeStep;
}

bool drumWasKicked(uint8_t i){
  bool sensorHeardSound = (analogRead(pinSoundSensor[i]) > sensorThreshold[i]);
  return sensorHeardSound;
}
