#include <stdint.h>

// total of connected cards
#define TOTAL_CARDS	4

// should be different for every slave
// range from 1 to TOTAL_CARDS - 1
#define CARD_NUMBER	1

#define LEDS_WIDTH_T	uint8_t
// should be large enought for the number of cards * height of a card
// = TOTAL_CARDS * 8
#define LEDS_HEIGHT_T	uint32_t

#define LEDS_WIDTH	8
#define LEDS_HEIGHT	TOTAL_CARDS * 8

// delay for slaves to wakeup
#define INITIAL_DELAY	1000
// delay between every iteration
#define TICK_MS		1000

// color of living pixel
#define COLOR_ON_R	255
#define COLOR_ON_G	0
#define COLOR_ON_B	0

// color of dead pixel
#define COLOR_OFF_R	0
#define COLOR_OFF_G	0
#define COLOR_OFF_B	0
