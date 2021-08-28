//#include <Arduino.h>
#include <stm8s_gpio.h>
#include "HX711.h"
#include <stm8s_exti.h>
#include "delay.h"

//#define bool smart_switch

//Filter parameters
#define WEIGHT_CNT 3 //HX711 speed is 80Hz or near this value. To get 10 values the sensor need around 120ms.
#define GISTERESIS 300
#define WEIGHT_THREASHOLD 5000 //Weight difference trigger. Lower is better

//Pins
#define PORT_LED GPIOB
#define PIN_LED GPIO_PIN_5 // its pin with real open drein and LED connected to Vdd (lighting then is 0)

#define PORT_TARE GPIOC
#define PIN_TARE GPIO_PIN_4

#define PORT_END_STOP GPIOC
#define PIN_END_STOP GPIO_PIN_5

// vars
static uint8_t tare_val;
static uint8_t tare_changed = 0;
static uint8_t end_stop_enable = 0;

long value;


 void fastBlink (uint8_t cnt){
 for (uint8_t i = 0; i < cnt; i++){
   if (i != 0) _delay_ms(100);
 GPIO_WriteLow(PORT_LED, PIN_LED); // enable led
 _delay_ms(50);
 GPIO_WriteHigh(PORT_LED, PIN_LED); // disable led
 }

} 

void setup(void)
{

  // pins configuration

  GPIO_Init(PORT_LED, PIN_LED, GPIO_MODE_OUT_OD_HIZ_SLOW);           // slow , led disabled
  GPIO_Init(PORT_TARE, PIN_TARE, GPIO_MODE_IN_PU_IT);                 // tare signal from printer
  GPIO_Init(PORT_END_STOP, PIN_END_STOP, GPIO_MODE_OUT_PP_HIGH_SLOW); // end_stop not conencted to 0 by default (end_stop disabled)

  tare_val = GPIO_ReadInputPin(PORT_TARE, PIN_TARE);

  fastBlink(3); // start_up info
  
  HX711_init(128);

  /* Initialize the Interrupt sensitivity */
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_RISE_FALL); // enable interupt for tare signal

  enableInterrupts();
}



void loop(void)
{

  fastBlink(2); // init compite info

  while (1)
  {
    // if tare signal up now , then  do tare
    if (tare_changed && tare_val)
    {
      tare_changed = 0; // reset flag
    //  fastBlink(1); // tare input info
      HX711_tare(3);
    }

    // wait to tare signal from printer to start meashurence or check thet was disabled
    if (!tare_val && !end_stop_enable)
      continue;

    value = HX711_get_mean_value(WEIGHT_CNT);

    if (value >= WEIGHT_THREASHOLD)
    {
      end_stop_enable = 1;
    }
    else if (end_stop_enable && value < WEIGHT_THREASHOLD - GISTERESIS)
    {

      end_stop_enable = 0;
    }

    if (end_stop_enable)
    {
      GPIO_WriteLow(PORT_LED, PIN_LED);           // led on
      GPIO_WriteLow(PORT_END_STOP, PIN_END_STOP); // end stop conenct to ground (activate)
    }
    else
    {

      GPIO_WriteHigh(PORT_LED, PIN_LED);           // led of
      GPIO_WriteHigh(PORT_END_STOP, PIN_END_STOP); // end stop conenct to Vdd (diactivate)
    }
  }
}

void main(void)
{
  setup();
  loop();
}

INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 // if (tare_val != GPIO_ReadInputPin(PORT_TARE,PIN_TARE)) {
  tare_val = GPIO_ReadInputPin(PORT_TARE, PIN_TARE);
  tare_changed = 1;
 // }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8 *file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
