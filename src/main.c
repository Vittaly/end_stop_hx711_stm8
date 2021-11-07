#include <PCD8544.h>
#include "HX711.h"
#include "delay.h"
#include "arduino.h"

//---- define pins---
#define HX711_DOUT PB4
#define HX711_CLK PC3
#define END_STOP PC5
#define TARE PC4

//Filter parameters
#define WEIGHT_CNT 2 //HX711 speed is 80Hz or near this value. To get 10 values the sensor need around 120ms.

#define WEIGHT_THREASHOLD 2000 //Weight difference trigger. Lower is better
#define GISTERESIS (WEIGHT_THREASHOLD / 10)
//#define INVERT_DIR // use if sensor mount incorrest

// should be disable after debugering
//#define DEBUG

#ifdef DEBUG
// disable sense for first debug wo sensor
//#define DISABLE_SENS

#define LCD_CLK PC6
#define LCD_SDIN PC7
#define LCD_DC PD1
#define LCD_RESET PD2
#define LCD_SCE PD3

PCD8544(lcd, LCD_CLK, LCD_SDIN, LCD_DC, LCD_RESET, LCD_SCE); // sclk,sdin,dc,reset,sce

#endif

//------------vars--------------------
//static unsigned char tare_val = 0;
//static unsigned char tare_val_old = 0;
//static unsigned char tare_standup = 0;
static unsigned char end_stop_enable = 0;
static long meas_value;

void fastBlink(unsigned char cnt)
{
  for (unsigned char i = 0; i < cnt; i++)
  {
    if (i != 0)
      _delay_ms(200);
    digitalWrite(PIN_LED_BUILTIN, LOW); // enable led
    _delay_ms(200);
    digitalWrite(PIN_LED_BUILTIN, HIGH); // disable led
  }
}

//------------------interupts-------------
// void on_tare_change(void)
// {
//   // tare_standup = HIGH;
//   //#define TARE_CNT 3
//   //for (char i = 0; i < TARE_CNT;i++){  
//     tare_val = digitalRead(TARE);
//    // _delay_us(30);
//   //}
//   //tare_val /= TARE_CNT;

//   if (tare_val != tare_val_old) // signal changed
//   {
//     #ifdef DEBUG
//     Serial_println_s("Tired change!!!");
//     #endif
//     tare_val_old = tare_val;
//     if (tare_val == HIGH)
//       tare_standup = HIGH; // start to work
//   }
// }

void setup(void)
{

  // pin conf
  pinMode(PIN_LED_BUILTIN, OUTPUT);
  pinMode(TARE, INPUT);
  pinMode(END_STOP, OUTPUT);
  digitalWrite(PIN_LED_BUILTIN, HIGH); // disable led

#ifdef DEBUG
  lcd_begin();
  HardwareSerial_begin(9600);
  //PCD8544_begin();
  // Write some text on the first line...
  lcd_setCursor(0, 0);
  lcd_print_s("Hello, World!");
  Serial_println_s("Hello, World!");

#endif

#ifndef DISABLE_SENS
  HX711_begin(HX711_DOUT, HX711_CLK, 128); // dout clk range
  HX711_tare(WEIGHT_CNT+2);
#endif
  //  attachInterrupt(digitalPinToPort(TARE), on_tare_change, CHANGE);
  //  EXTI_SetExtIntSensitivity( EXTI_PORT_GPIOC, EXTI_SENSITIVITY_RISE_FALL);

  //GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_FL_IT);
  // enableInterrupts();

  fastBlink(3); // start_up info
}

void loop()
{
  //on_tare_change();


  // if tare signal up now , then  do tare
  if (digitalRead(TARE))
  {
 //   tare_standup = LOW; // reset flag
    fastBlink(1);       // tare input info

#ifndef DISABLE_SENS
    HX711_tare(WEIGHT_CNT+1);
#endif

#ifdef DEBUG
    Serial_println_s("Tired:");
    Serial_println_s(HX711_get_offset());
#endif
  }

  // wait to tare signal from printer to start meashurence or check thet was disabled
  //if (!tare_val && !end_stop_enable)    return;



#ifndef DISABLE_SENS
  meas_value = HX711_get_mean_value(WEIGHT_CNT);
#else
  meas_value = 567;
#endif
#ifdef DEBUG
  _delay_ms(1000);
  //lcd_setCursor(0, 1);
  //lcd_print_u(meas_value);
  Serial_print_s("cur_v=");
  Serial_print_i(HX711_read_average(1));
  Serial_print_s("::ofs=");
  Serial_print_i(HX711_get_offset());
 //   Serial_print_s("::tres=");
 // Serial_print_i(WEIGHT_THREASHOLD);
    Serial_print_s("::end=");
  Serial_print_i(end_stop_enable);
   // Serial_print_s("::tare_val=");
 // Serial_print_i(tare_val);
  Serial_print_s("::val=");
  Serial_println_i(meas_value);
#endif
#ifndef INVERT_DIR
  if (meas_value >= WEIGHT_THREASHOLD)
#else
 if (meas_value <= -WEIGHT_THREASHOLD)
 #endif
  {
    end_stop_enable = 1;
  }
  else 
  #ifndef INVERT_DIR
  if (end_stop_enable && meas_value < (WEIGHT_THREASHOLD - GISTERESIS))
  #else
  if (end_stop_enable && meas_value > -(WEIGHT_THREASHOLD - GISTERESIS))
  #endif
  {

    end_stop_enable = 0;
  }

  if (end_stop_enable)
  {
    digitalWrite(PIN_LED_BUILTIN, LOW); // led on
    digitalWrite(END_STOP, HIGH);        // end stop conenct to to 3.3v (activate)
  }
  else
  {

    digitalWrite(PIN_LED_BUILTIN, HIGH); // led of
    digitalWrite(END_STOP, LOW);        // end stop conenct to ground (diactivate)
  }
}