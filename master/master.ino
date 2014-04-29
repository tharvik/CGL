#include "config.h"

// array containing states of the game
// to avoid many memory copy, we use two arrays and just update current
static uint8_t current = 0;
static LEDS_HEIGHT_T arrays[2][LEDS_WIDTH] = {{
	0x0000, 0x2744, 0x0244, 0x2244, 0x227C, 0x2244, 0x2744, 0x0000
}};

/// end of configuration

// return state of the given pixel
static bool get_pixel(LEDS_WIDTH_T const x, LEDS_HEIGHT_T const y)
{
	return arrays[current][x] & (0x1 << y);
}

// set the given pixel to the given state in the other array
static void set_pixel(LEDS_WIDTH_T const x, LEDS_HEIGHT_T const y,
		bool const state)
{
	uint8_t const index = (current ? 0 : 1);
	LEDS_HEIGHT_T byte = arrays[index][x];

	if(state)
		byte = byte | (1 << y);
	else
		byte = byte & ~(1 << y);

	arrays[index][x] = byte;
}

// return the number of live neighbors of the given cell
static uint8_t living_neigh(uint8_t const x, uint8_t const y)
{
	uint8_t count = 0;

	// loop to check for all neighbours
	for(int8_t i = - 1; i <= 1 ; i++) {
		for(int8_t j = - 1; j <= 1 ; j++) {

			// jump iself
			if(i == 0 && j == 0)
				continue;

			// some basic checks to handle corner cases
			if(i + x >= 0 && j + y >= 0 &&
				i + x < LEDS_WIDTH && j + y < LEDS_HEIGHT)
				count += get_pixel(i + x, j + y);
		}
	}

	return count;
}

// apply the rules to the given cell and output to out
static void rule(uint8_t const x, uint8_t const y)
{
	uint8_t const neighs = living_neigh(x, y);
	bool const cell = get_pixel(x, y);

	// as wikipedia
	if((!cell && neighs == 3) || (cell && (neighs == 2 || neighs == 3)))
		set_pixel(x, y, true);
	else
		set_pixel(x, y, false);
}

// update the array
static void evolve(void)
{
	// apply rule to every pixel
	for(uint8_t x = 0; x < LEDS_WIDTH; x++)
		for(uint8_t y = 0; y < LEDS_HEIGHT; y++)
			rule(x, y);

	// use new array
	current = (current ? 0 : 1);
}

// send new screen to salve
#include <Wire.h>
static void update_slave(void)
{
	// create an array of byte
	uint8_t array[LEDS_WIDTH];
	for(uint8_t x = 0; x < LEDS_WIDTH; x++) {

		// get only needed part
		uint8_t const byte = arrays[current][x] >> (LEDS_HEIGHT / 2);
		array[x] = byte;
	}

#ifdef DEBUG
	for(uint8_t x = 0; x < LEDS_WIDTH; x++) {
		Serial.print(array[x]);
		Serial.println();
	}
	Serial.println("--");
#endif

	// send the data
	Wire.beginTransmission(I2C_ADRESS);
	Wire.write(array, LEDS_WIDTH);
	Wire.endTransmission();
}

// display the current state of the array
#include <Colorduino.h>
static void display(void)
{
	PixelRGB *p = Colorduino.GetPixel(0, 0);

	for(uint8_t x = 0; x < LEDS_WIDTH; x++) {
		for(uint8_t y = 0; y < LEDS_HEIGHT / 2; y++, p++) {

			// set on only if pixel is on
			if(get_pixel(x, y)) {
				p->r = COLOR_ON_R;
				p->g = COLOR_ON_G;
				p->b = COLOR_ON_B;
			} else {
				p->r = COLOR_OFF_R;
				p->g = COLOR_OFF_G;
				p->b = COLOR_OFF_B;
			}
		}
	}

	// refresh screen
	Colorduino.FlipPage();
}

// setup the card, set the array at false
#include <Wire.h>
#include <Colorduino.h>
void setup(void)
{
#ifdef DEBUG
	Serial.begin(BAUD);
#endif
	Wire.begin();
	Colorduino.Init();

	// wait for slave to start
	delay(INITIAL_DELAY);
	display();
	update_slave();
	delay(TICK_MS);
}

// main loop
void loop(void)
{

#ifdef DEBUG
	for(uint8_t x = 0; x < LEDS_WIDTH; x++) {
		for(uint8_t y = 0; y < LEDS_HEIGHT; y++) {
			Serial.print(get_pixel(x, y));
		}
		Serial.println();
	}
	Serial.println("--");
#endif

	evolve();
	update_slave();
	display();

	delay(TICK_MS);
}
