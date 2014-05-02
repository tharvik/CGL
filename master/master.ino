#include "config.h"

// array containing states of the game
// to avoid many memory copy, we use two arrays and just update current
static uint8_t current = 0;
static LEDS_HEIGHT_T arrays[2][LEDS_WIDTH] = {{
	0x00000000, 0x00274400, 0x00024400, 0x00224400,
	0x00227C00, 0x00224400, 0x00274400, 0x00000000
}};

/// end of configuration

// return state of the given pixel
static bool get_pixel(uint8_t const x, uint8_t const y)
{
	return bitRead(arrays[current][x], y);
}

// set the given pixel to the given state in the other array
static void set_pixel(uint8_t const x, uint8_t const y,
		bool const state)
{
	uint8_t const index = (current ? 0 : 1);
	bitWrite(arrays[index][x], y, state);
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
				i + x < (signed) LEDS_WIDTH &&
				j + y < (signed) LEDS_HEIGHT)
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
static void update_slaves(void)
{
	// create an array of byte for every cards
	uint8_t array[TOTAL_CARDS - 1][LEDS_WIDTH];
	for(uint8_t i = 1; i < TOTAL_CARDS; i++) {
		for(uint8_t x = 0; x < LEDS_WIDTH; x++) {

			// get only needed part
			uint8_t const byte = arrays[current][x] >>
				(i * LEDS_HEIGHT / TOTAL_CARDS);
			array[i - 1][x] = byte;
		}
	}

#ifdef DEBUG
	for(uint8_t i = 0; i < TOTAL_CARDS - 1; i++) {
		Serial.print("Slave ");
		Serial.println(i);
		for(uint8_t x = 0; x < LEDS_WIDTH; x++) {
			Serial.print(array[i][x]);
			Serial.println();
		}
	}
	Serial.println("--");
#endif

	// send the data
	for(uint8_t i = 1; i < TOTAL_CARDS; ++i) {
		Wire.beginTransmission(i);
		Wire.write(array[i - 1], LEDS_WIDTH);
		Wire.endTransmission();
	}
}

// display the current state of the array
#include <Colorduino.h>
static void display(void)
{
	PixelRGB *p = Colorduino.GetPixel(0, 0);

	for(uint8_t x = 0; x < LEDS_WIDTH; x++) {
		for(uint8_t y = 0; y < LEDS_HEIGHT / TOTAL_CARDS; y++, p++) {

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
	update_slaves();
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
	update_slaves();
	display();

	delay(TICK_MS);
}
