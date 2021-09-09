#include <PCD8544.h>
#include "HX711.h"
#include "delay.h"

//---- define pins---
#define HX711_DOUT PD4
#define HX711_CLK PC3
#define END_STOP PC5
#define TARE PC4

//Filter parameters
#define WEIGHT_CNT 3 //HX711 speed is 80Hz or near this value. To get 10 values the sensor need around 120ms.
#define GISTERESIS 300
#define WEIGHT_THREASHOLD 1000 //Weight difference trigger. Lower is better

// should be disable after debugering
#define DEBUG

#ifdef DEBUG
// disable sense for first debug wo sensor
#define DISABLE_SENS

#define LCD_CLK PC6
#define LCD_SDIN PC7
#define LCD_DC PD1
#define LCD_RESET PD2
#define LCD_SCE PD3

PCD8544(lcd, LCD_CLK, LCD_SDIN, LCD_DC, LCD_RESET, LCD_SCE); // sclk,sdin,dc,reset,sce


#endif

//------------vars--------------------
static uint8_t tare_val;
static uint8_t tare_changed = 0;
static uint8_t end_stop_enable = 0;
static long meas_value;

void fastBlink(uint8_t cnt)
{
  for (uint8_t i = 0; i < cnt; i++)
  {
    if (i != 0)
      _delay_ms(100);
    digitalWrite(PIN_LED_BUILTIN, LOW); // enable led
    _delay_ms(50);
    digitalWrite(PIN_LED_BUILTIN, HIGH); // disable led
  }
}

//------------------interupts-------------
void on_tare_change(void)
{
  tare_changed = HIGH;
  tare_val = digitalRead(TARE);
}

void setup(void)
{

  // pin conf
  pinMode(PIN_LED_BUILTIN, OUTPUT);

#ifdef DEBUG
  lcd_begin();
  Serial_begin(9600);
  //PCD8544_begin();
  // Write some text on the first line...
  lcd_setCursor(0, 0);
  lcd_print_s("Hello, World!");
  Serial_println_s("Hello, World!");
#endif

#ifndef DISABLE_SENS
  HX711_begin(PD4, PC5, 128); // dout clk range
#endif
  //  attachInterrupt(digitalPinToInterrupt(TARE), on_tare_change, CHANGE);
  fastBlink(3); // start_up info
}

void loop()
{

  // if tare signal up now , then  do tare
  if (tare_changed && tare_val)
  {
    tare_changed = 0; // reset flag
                      //  fastBlink(1); // tare input info
                      #ifndef DISABLE_SENS
    HX711_tare(3);
    #endif
  }

  // wait to tare signal from printer to start meashurence or check thet was disabled
  if (!tare_val && !end_stop_enable)
    return;
#ifndef DISABLE_SENS
  meas_value = HX711_get_mean_value(WEIGHT_CNT);
  #else
  meas_value = 567;
  #endif
#ifdef DEBUG
  _delay_ms(500);
  lcd_setCursor(0, 1);
  lcd_print_u(meas_value);  
  Serial_println_u(meas_value);
#endif

  if (meas_value >= WEIGHT_THREASHOLD)
  {
    end_stop_enable = 1;
  }
  else if (end_stop_enable && meas_value < WEIGHT_THREASHOLD - GISTERESIS)
  {

    end_stop_enable = 0;
  }

  if (end_stop_enable)
  {
    digitalWrite(PIN_LED_BUILTIN, LOW);           // led on
    digitalWrite(END_STOP, LOW); // end stop conenct to ground (activate)
  }
  else
  {

    digitalWrite(PIN_LED_BUILTIN, HIGH);            // led of
    digitalWrite(END_STOP, HIGH); // end stop conenct to 3.3v (diactivate)
  }
}