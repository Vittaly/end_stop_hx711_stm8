//#include <Arduino.h>
#include<stm8s_gpio.h>
#include"HX711.h"




//Filter parameters
#define WEIGHT_CNT 3 //HX711 speed is 80Hz or near this value. To get 10 values the sensor need around 120ms.
#define GISTERESIS 100 
#define WEIGHT_TREASHOLD 3000 //Weight difference trigger. Lower is better







void setup (void) 
{GPIO_WriteHigh(GPIOA, GPIO_PIN_4);

HX711_init(128);

// ---------- TIM4 CONFIG -----------------------
    TIM4_DeInit();
    enableInterrupts();

}

void loop (void) {
//  GPIO_WriteHigh(GPIOA, GPIO_PIN_4);

while (1){



}

}


void main (void){
 setup();
  loop();
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
