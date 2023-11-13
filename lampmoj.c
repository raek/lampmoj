#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <util/delay.h>

#define T 100
#define LED_MAX 63
#define COLOR_CYCLE_LENGTH ((LED_MAX+1) * 6)

// PIN ALLOCATION
//
// Red   LED: PB0, PWM'ed by OC0A, active low
// Green LED: PB1, PWM'ed by OC0B, active low
// Blue  LED: PB4, PWM'ed by OC1B, active low
//
// White/color switch: PB2, input, pullup enabled
// Run/stop switch:    PB3, input, pullup enabled

uint8_t intensity_lut[64] = {
/*[[[cog
import cog
for i in range(63):
    value = int((i / 4) ** 2)
    cog.outl(f"    {255 - value},")
cog.outl("    0");
]]]*/
255,
255,
255,
255,
254,
254,
253,
252,
251,
250,
249,
248,
246,
245,
243,
241,
239,
237,
235,
233,
230,
228,
225,
222,
219,
216,
213,
210,
206,
203,
199,
195,
191,
187,
183,
179,
174,
170,
165,
160,
155,
150,
145,
140,
134,
129,
123,
117,
111,
105,
99,
93,
86,
80,
73,
66,
59,
52,
45,
38,
30,
23,
15,
0
//[[[end]]]
};

void setup_leds(void)
{
    // Set LED pins to outputs
    DDRB |= (1<<0) | (1<<1) | (1<<4);
    // Turn off LEDs (pins low)
    PORTB |= (1<<0) | (1<<1) | (1<<4);

    // COM0A=10 (clear OC0A on match), COM0B=10, WGM0=011 (fast PWM)
    TCCR0A = 0xAF;
    // WGM0=011, CS0=011 (prescaler=64)
    TCCR0B = 0x03;
    OCR0A = 0xFF;
    OCR0B = 0xFF;

    // CS1=0111 (prescaler=64)
    TCCR1 = 0x07;
    // PWM1B=1, COM1B=10 (clear OC1B on match)
    GTCCR = 0x60;
    OCR1B = 0xFF;
}

void set_leds(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0A = intensity_lut[r];
    OCR0B = intensity_lut[g];
    OCR1B = intensity_lut[b];
}

void color_cycle(uint16_t t)
{
    uint8_t part = t / (LED_MAX+1);
    uint8_t phase = t % (LED_MAX+1);

    if (part == 0) {
	// Green up
	set_leds(LED_MAX, phase, 0);
    } else if (part == 1) {
	// Red down
	set_leds(LED_MAX-phase, LED_MAX, 0);
    } else if (part == 2) {
	// Blue up
	set_leds(0, LED_MAX, phase);
    } else if (part == 3) {
	// Green down
	set_leds(0, LED_MAX-phase, LED_MAX);
    } else if (part == 4) {
	// Red up
	set_leds(phase, 0, LED_MAX);
    } else if (part == 5) {
	// Blue down
	set_leds(LED_MAX, 0, LED_MAX-phase);
    } else {
	set_leds(0, 0, 0);
    }
}

void setup_switches(void)
{
    // Set switch pins to inputs (also the default)
    DDRB &= ~((1<<2) | (1<<3));
    // Enable pull-ups for switch pins
    PORTB |= (1<<2) | (1<<3);
}

bool get_color_cycle_mode_enabled(void)
{
    return (PINB & (1<<2)) != 0;
}

int main(int argc, char **argv)
{
    uint16_t t = 0;

    setup_leds();
    setup_switches();
    for (;;) {
	if (get_color_cycle_mode_enabled()) {
	    // Cycle colors
	    color_cycle(t);
	    _delay_ms(T);
	    t++;
	    if (t >= COLOR_CYCLE_LENGTH) {
		t = 0;
	    }
	} else {
	    // Constantly white
	    set_leds(LED_MAX, LED_MAX, LED_MAX);
	}
    }
}
