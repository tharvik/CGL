#include <stdint.h>

#define LEDS_WIDTH_T	uint8_t
#define LEDS_HEIGHT_T	uint16_t

#define LEDS_WIDTH	sizeof(LEDS_WIDTH_T)	* 8
#define LEDS_HEIGHT	sizeof(LEDS_HEIGHT_T)	* 8

#define INITIAL_DELAY	100
#define TICK_MS		10000

#define I2C_ADRESS	2

#define COLOR_ON_R	255
#define COLOR_ON_G	0
#define COLOR_ON_B	0

#define COLOR_OFF_R	0
#define COLOR_OFF_G	0
#define COLOR_OFF_B	0

#define BAUD		9600
