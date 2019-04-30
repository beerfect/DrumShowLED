#include <FastLED.h>    //подключаем библиотеку
#define NUM_LEDS 60     // общее количество светодиодов
#define DATA_PIN 5      // пин подключенный к ленте
CRGB leds[NUM_LEDS];    // массив для манипуляции светодиодами

float brigtness = 0;    // дефолтная яркость
float blackoutStep = 5; //доступные шаги затемнения: 0.5, 1, 3, 5, 15, 17, 42.5 

int color1 = 20;         // цвет ленты 1
int color2 = 120;        // цвет ленты 2
int color3 = 190;       // цвет ленте 3

const int buttonPin = 2; // Куда подключена кнопка

void setup() {
  pinMode(buttonPin, INPUT); // Вход кнопки
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // декларируем библиотеке - к пину DATA_PIN подключены NUM_LEDS диодов, ассоциированы с массивом leds
}

void loop() {

  // по умолчанию все диоды имеют яркость = 0 (полностью темные)
  fill_solid( &(leds[0]), NUM_LEDS / 3, CHSV( color1, 255, brigtness ) );
  fill_solid( &(leds[NUM_LEDS / 3 ]), NUM_LEDS / 3, CHSV( color2, 255, brigtness));
  fill_solid( &(leds[NUM_LEDS - NUM_LEDS / 3]), NUM_LEDS / 3, CHSV( color3, 255, brigtness) );

  // по нажатию кнопки устанавливаем яркость на максимум
  if (digitalRead(buttonPin)) {
    brigtness = 255;
    FastLED.show();
    while (digitalRead(buttonPin)) {}
  }

  // постепенно затемняем
  while (brigtness > 0) {
    brigtness -= blackoutStep;
    fill_solid( &(leds[0]), NUM_LEDS / 3, CHSV( color1, 255, brigtness) );
    fill_solid( &(leds[NUM_LEDS / 3 ]), NUM_LEDS / 3, CHSV( color2, 255, brigtness));
    fill_solid( &(leds[NUM_LEDS - NUM_LEDS / 3]), NUM_LEDS / 3, CHSV( color3, 255, brigtness) );
    FastLED.show();

    // если нажали во время затемнения - выход из цикла 
    if (digitalRead(buttonPin)) {
      break;
    }
  }
}