/**
 *
 * HX711 library for Arduino
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#ifndef HX711_h
#define HX711_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// static byte PD_SCK;		// Power Down and Serial Clock Input Pin
// static byte DOUT;		// Serial Data Output Pin
// static byte GAIN;		// amplification factor
// static long OFFSET = 0; // used for tare weight
// static float SCALE = 1; // used to return weight in grams, kg, ounces, whatever

void HX711_begin(byte dout, byte pd_sck, byte gain);

uint8_t HX711_is_ready();

void HX711_set_gain(uint8_t gain);

long HX711_read();

long HX711_read_average(uint8_t times);

long HX711_get_value();

long HX711_get_mean_value(uint8_t times);

void HX711_tare(uint8_t times);

void HX711_set_offset(long offset);

long HX711_get_offset();

void HX711_power_down();

void HX711_power_up();

#endif /* HX711_h */
