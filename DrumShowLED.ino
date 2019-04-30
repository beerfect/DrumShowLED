#include <FastLED.h>                                    //подключаем библиотеку
#define NUM_LEDS 60                                     // общее количество светодиодов
#define DATA_PIN 5                                      // пин подключенный к ленте
//#define BRIGHTNESS 0
CRGB leds[NUM_LEDS];                                    // массив для манипуляции светодиодами

float brigtness = 0;
float dimStep = 5; //0.5, 1, 3, 5, 15, 17, 42.5

int color1 = 0;
int color2 = 96;
int color3 = 160;

const int buttonPin = 2; // Куда подключена кнопка

void setup() {
  pinMode(buttonPin, INPUT); // Вход кнопки
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // декларируем библиотеке - к пину DATA_PIN подключены NUM_LEDS диодов, ассоциированы с массивом leds
}

void loop() {

  fill_solid( &(leds[0]), NUM_LEDS / 3, CHSV( color1, 255, brigtness ) );
  fill_solid( &(leds[NUM_LEDS / 3 ]), NUM_LEDS / 3, CHSV( color2, 255, brigtness));
  fill_solid( &(leds[NUM_LEDS - NUM_LEDS / 3]), NUM_LEDS / 3, CHSV( color3, 255, brigtness) );

  //  FastLED.show();

  if (digitalRead(buttonPin)) {
    brigtness = 255;
    FastLED.show();
    while (digitalRead(buttonPin)) {}
  }
  while (brigtness > 0) {
    brigtness -= dimStep;
    fill_solid( &(leds[0]), NUM_LEDS / 3, CHSV( color1, 255, brigtness) );
    fill_solid( &(leds[NUM_LEDS / 3 ]), NUM_LEDS / 3, CHSV( color2, 255, brigtness));
    fill_solid( &(leds[NUM_LEDS - NUM_LEDS / 3]), NUM_LEDS / 3, CHSV( color3, 255, brigtness) );

    FastLED.show();
    if (digitalRead(buttonPin)) {
      break;
    }
  }



}






//  if (digitalRead(buttonPin)) {
//    for (int i = 256; i >= 0 ; i -= 1) {
//      fill_solid( &(leds[0]), NUM_LEDS / 3, CHSV( 0, 255, i) );
//      fill_solid( &(leds[NUM_LEDS / 3 ]), NUM_LEDS / 3, CHSV( 96, 255, i) );
//      fill_solid( &(leds[NUM_LEDS - NUM_LEDS / 3]), NUM_LEDS / 3, CHSV( 160, 255, i) );
//      FastLED.show();
//    }
//  }
