//#include <Arduino.h>
#include<stm8s_gpio.h>
#include"HX711.h"



void setup (void) 
{GPIO_WriteHigh(GPIOA, GPIO_PIN_4);

HX711_init(128);

}

void loop (void) {
  GPIO_WriteHigh(GPIOA, GPIO_PIN_4);
}


void main (void){
 setup();
  loop();
}