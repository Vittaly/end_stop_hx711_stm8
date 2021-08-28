#include <stdint.h>

//==== config ====


 //==== pins=======
  #define PORT_DOUT GPIOB
  #define PIN_DOUT GPIO_PIN_4 //B4 and B5 is 5v tolerant input (no protect diode to Vss)


  #define PORT_CLK GPIOC
  #define PIN_CLK GPIO_PIN_3
 //==== end pins=======

#define STROBE_WIGHT 2


// define channel, and gain factor
// channel selection is made by passing the appropriate gain: 128 or 64 for channel A, 32 for channel B
void HX711_init(uint8_t gain);

// check if HX711 is ready
// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
uint8_t HX711_is_ready();

// set the gain factor; takes effect only after a call to read()
// channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
// depending on the parameter, the channel is also set to either A or B
void HX711_set_gain(uint8_t gain);

// waits for the chip to be ready and returns a reading
long HX711_read();

// returns an average reading; times = how many times to read
long HX711_read_average(uint8_t times);

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
long HX711_get_value();
long HX711_get_mean_value(uint8_t times);


// returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
// times = how many readings to do
float HX711_get_units();
float HX711_get_mean_units(uint8_t times);

// set the OFFSET value for tare weight; times = how many times to read the tare value
void HX711_tare(uint8_t times);

// set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
void HX711_set_scale(float scale);

// get the current SCALE
float HX711_get_scale();

// set OFFSET, the value that's subtracted from the actual reading (tare weight)
void HX711_set_offset(long offset);

// get the current OFFSET
long HX711_get_offset();

// puts the chip into power down mode
void HX711_power_down();

// wakes up the chip after power down mode
void HX711_power_up();


