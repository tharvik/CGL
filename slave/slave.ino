#include "config.h"

#include <Wire.h>

// current line received
static uint8_t line = 0;

// callback for new data from master
#include <Colorduino.h>
static void new_screen(int const bytes_size)
{
	// get a new pointer to write
	PixelRGB *p = Colorduino.GetPixel(line, 0);

#ifdef DEBUG
	LEDS_WIDTH_T const array[LEDS_WIDTH] = {2, 0, 0, 0, 224, 0, 0, 0};

	for(uint8_t i = 0; i < LEDS_WIDTH; i++) {
		LEDS_WIDTH_T byte = array[i];
#else
	for(uint8_t i = 0; i < bytes_size; i++) {
		LEDS_WIDTH_T byte = Wire.read();
#endif
		// loop to print whole byte
		for(uint8_t x = 0; x < LEDS_HEIGHT / 2; x++, p++) {

			bool const on = (byte >> x) & 0x1;
			if(on) {
				p->r = COLOR_ON_R;
				p->g = COLOR_ON_G;
				p->b = COLOR_ON_B;
			} else {
				p->r = COLOR_OFF_R;
				p->g = COLOR_OFF_G;
				p->b = COLOR_OFF_B;
			}
		}
		line = (line + 1) % (LEDS_HEIGHT / 2);
	}

	// only flip at end of loop
	if(line == 0)
		Colorduino.FlipPage();
}

// setup the card, mainly opening for I2C communication
#include <Colorduino.h>
void setup()
{
	Wire.begin(I2C_ADRESS);
	Wire.onReceive(new_screen);
	Colorduino.Init();
}

// do nothing, just wait for callback
void loop()
{
	// empty
#ifdef DEBUG
	new_screen(0);
	delay(TICK_MS);
#endif
}
