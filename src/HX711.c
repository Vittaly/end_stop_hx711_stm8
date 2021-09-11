#include "HX711.h"
#include <Arduino.h>
#include "delay.h"

static uint8_t PD_SCK;    // Power Down and Serial Clock Input Pin
static uint8_t DOUT;// Serial Data Output Pin
static uint8_t GAIN;// amplification factor
static long OFFSET = 0;    // used for tare weight
static float SCALE = 1;    // used to return weight in grams, kg, ounces, whatever
 
#define STROBE_WIGHT 1
#define clock_low() digitalWrite(PD_SCK, LOW)
#define clock_high() digitalWrite(PD_SCK, HIGH)
#define get_DOUT() digitalRead(DOUT)

 
void HX711_begin(byte dout, byte pd_sck, byte gain){
    PD_SCK = pd_sck;
	DOUT = dout;

	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);

	HX711_set_gain(gain);
} 


uint8_t HX711_is_ready() {
    return (get_DOUT() == LOW);
}

void HX711_set_gain(uint8_t gain) {
    switch (gain) {
        case 128:        // channel A, gain factor 128
            GAIN = 1;
            break;
        case 64:        // channel A, gain factor 64
            GAIN = 3;
            break;
        case 32:        // channel B, gain factor 32
            GAIN = 2;
            break;
    }

    //digitalWrite(PD_SCK, LOW);
    clock_low();
    HX711_read();
}



long HX711_read() {
    // wait for the chip to become ready
    while (!HX711_is_ready());

    unsigned long value = 0;
    uint8_t data[3] = { 0 };
    uint8_t filler = 0x00;

    // pulse the clock pin 24 times to read the data
    for(int8_t n = 2; n>=0; n--) {
        for(int8_t i=7; i>=0; i--) {
            clock_high();
            _delay_us(STROBE_WIGHT); // let some time to hx711 to update output value
            data[n] |= get_DOUT() << i;
            clock_low();
		//	_delay_us(STROBE_WIGHT); // let some time to hx711 to update output value
        }
    }
    // set the channel and the gain factor for the next reading using the clock pin
    for (uint8_t i = 0; i < GAIN; i++) {
        clock_high();
        _delay_us(STROBE_WIGHT); // let some time to hx711 to understand the command
        clock_low();
		_delay_us(STROBE_WIGHT); // let some time to hx711 to understand the command
    }

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if (data[2] & 0x80) {
        filler = 0xFF;
    } else {
        filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = ( (unsigned long)(filler) << 24
            | (unsigned long)(data[2]) << 16
            | (unsigned long)(data[1]) << 8
            | (unsigned long)(data[0]) );

    return (long)(value);
}

long HX711_read_average(uint8_t times) {
    long sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += HX711_read();
    }
    return sum / times;
}

long HX711_get_value() {
    return HX711_read_average(1) - OFFSET;
}

long HX711_get_mean_value(uint8_t times) {
    return HX711_read_average(times) - OFFSET;
}


void HX711_tare(uint8_t times) {
    long sum = HX711_read_average(times);
    HX711_set_offset(sum);
}



void HX711_set_offset(long offset) {
    OFFSET = offset;
}

long HX711_get_offset() {
    return OFFSET;
}

void HX711_power_down() {
    clock_low();
    clock_high();
}

void HX711_power_up() {
    clock_low();
}
