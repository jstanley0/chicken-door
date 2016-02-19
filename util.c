#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "util.h"

// Put the processor in idle mode for the specified number of "kiloclocks"
// (= periods of 1024 clock cycles)
volatile uint8_t wakeup;
ISR(TIMER1_COMPA_vect)
{
	wakeup = 1;
}
void Sleep_kc(uint16_t kiloclocks)
{
	TCCR1A = 0;
	TCCR1B = 0;				// stop the timer
	TIFR1 = (1 << OCF1A);	// clear output-compare A flag
	OCR1A = kiloclocks;		// set compare match A target
	TCNT1 = 0;				// reset timer counter
	TIMSK1 = (1 << OCIE1A);	// enable compare match A interrupt
	TCCR1B = (1 << CS12) | (1 << CS10);	// start timer with 1/1024 prescaler

	// sleep until it's time to wake up
	// use a loop here because other interrupts will happen
	wakeup = 0;
	set_sleep_mode(SLEEP_MODE_IDLE);
	do {
		sleep_mode();
	} while( !wakeup );

	TIMSK1 = 0;				// stop the interrupt
	TCCR1B = 0;				// stop the timer
}

void InitButtons(ButtonState *state, volatile uint8_t *pin, volatile uint8_t *port, 
                 volatile uint8_t *ddr, uint8_t mask)
{
    state->pin = pin;
    state->mask = mask;
    state->prevState = 0xFF;
    state->repeat = 0;
    
    (*port) |= mask;  // enable pull-up resistors
    (*ddr) &= ~mask; // set ports as inputs
}

uint16_t GetButtons(ButtonState *state)
{
	uint8_t curState = (*(state->pin) & state->mask); // 1 = not pressed; 0 = pressed
	
	// if we've already registered a "hold"
	if (state->repeat >= REPEAT_THRESHOLD) {
		state->prevState = curState;
		if (curState == state->mask)
			state->repeat = 0;	// no buttons are down.
		return 0;
	}

	if (curState != state->prevState) {
		// truth table:
		//  prev cur  released
		//  0    0    0
		//  0    1    1
		//  1    0    0
		//  1    1    0
		uint8_t released = ~state->prevState & curState & state->mask;
		state->prevState = curState;
		return released;
	} else if (curState != state->mask) {
		// button(s) are being held
		if (++state->repeat == REPEAT_THRESHOLD) {
			return BUTTON_HOLD | (~curState & state->mask);
		}
	} 
	
	return 0;
}
