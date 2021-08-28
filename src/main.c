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

 #define DEBUG

#ifdef DEBUG
#include "stm8s_uart1.h"
#include <stdio.h>

/**
  * @addtogroup UART1_Printf
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef _RAISONANCE_
  #define PUTCHAR_PROTOTYPE int putchar (char c)
  #define GETCHAR_PROTOTYPE int getchar (void)
#elif defined (_COSMIC_)
  #define PUTCHAR_PROTOTYPE char putchar (char c)
  #define GETCHAR_PROTOTYPE char getchar (void)
#elif defined (_SDCC_)         /* SDCC patch: ensure same types as stdio.h */
  #if SDCC_VERSION >= 30605      // declaration changed in sdcc 3.6.5 (officially with 3.7.0)
    #define PUTCHAR_PROTOTYPE int putchar(int c)
    #define GETCHAR_PROTOTYPE int getchar(void)
  #else
    #define PUTCHAR_PROTOTYPE void putchar(char c)
    #define GETCHAR_PROTOTYPE char getchar(void)
  #endif 
#else /* _IAR_ */
  #define PUTCHAR_PROTOTYPE int putchar (int c)
  #define GETCHAR_PROTOTYPE int getchar (void)
#endif /* _RAISONANCE_ */

PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);

  return (c);
}

/**
  * @brief Retargets the C library scanf function to the USART.
  * @param None
  * @retval char Character to Read
  */
GETCHAR_PROTOTYPE
{
#ifdef _COSMIC_
  char c = 0;
#else
  int c = 0;
#endif
  /* Loop until the Read data register flag is SET */
  while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
    c = UART1_ReceiveData8();
  return (c);
}

#endif




// vars
static uint8_t tare_val;
static uint8_t tare_changed = 0;
static uint8_t end_stop_enable = 0;

long value;

void fastBlink(uint8_t cnt)
{
  for (uint8_t i = 0; i < cnt; i++)
  {
    if (i != 0)
      _delay_ms(100);
    GPIO_WriteLow(PORT_LED, PIN_LED); // enable led
    _delay_ms(50);
    GPIO_WriteHigh(PORT_LED, PIN_LED); // disable led
  }
}

void setup(void)
{

  // pins configuration

  GPIO_Init(PORT_LED, PIN_LED, GPIO_MODE_OUT_OD_HIZ_SLOW);            // slow , led disabled
  #ifdef DEBUG
  GPIO_Init(PORT_TARE, PIN_TARE, GPIO_MODE_IN_PU_IT);                 // tare signal from printer, pull up in DEBUG
  #else
  GPIO_Init(PORT_TARE, PIN_TARE, GPIO_MODE_IN_FL_IT);                 // tare signal from printer
  #endif
  GPIO_Init(PORT_END_STOP, PIN_END_STOP, GPIO_MODE_OUT_PP_HIGH_SLOW); // end_stop not conencted to 0 by default (end_stop disabled)

  //uart
  #ifdef DEBUG
  UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    #endif

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
#ifdef DEBUG
    _delay_ms(200);
#endif
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
    #ifdef DEBUG
    printf("value: %n", value);
    #endif

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
