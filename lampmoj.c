#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

#define T 100
#define LED_MAX 63

// Red   LED: PB0, PWM'ed by OC0A, active low
// Green LED: PB1, PWM'ed by OC0B, active low
// Blue  LED: PB4, PWM'ed by OC1B, active low

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

int main(int argc, char **argv)
{
    setup_leds();
    set_leds(LED_MAX, 0, 0);
    for (;;) {
	// Green up
	for (uint8_t i = 0; i < LED_MAX; i++) {
	    set_leds(LED_MAX, i, 0);
	    _delay_ms(T);
	}
	// Red down
	for (uint8_t i = LED_MAX-1; i > 0; i--) {
	    set_leds(i, LED_MAX, 0);
	    _delay_ms(T);
	}
	// Blue up
	for (uint8_t i = 0; i < LED_MAX; i++) {
	    set_leds(0, LED_MAX, i);
	    _delay_ms(T);
	}
	// Green down
	for (uint8_t i = LED_MAX-1; i > 0; i--) {
	    set_leds(0, i, LED_MAX);
	    _delay_ms(T);
	}
	// Red up
	for (uint8_t i = 0; i < LED_MAX; i++) {
	    set_leds(i, 0, LED_MAX);
	    _delay_ms(T);
	}
	// Blue down
	for (uint8_t i = LED_MAX-1; i > 0; i--) {
	    set_leds(LED_MAX, 0, i);
	    _delay_ms(T);
	}
    }
}
