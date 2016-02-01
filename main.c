// chicken coop door opener firmware
// built around AVR ATMEGAxx8
// build with avr-gcc
//
// Source code (c) 2007-2016 by Jeremy Stanley
// Licensed under GNU GPL v2 or later

// Port assignments:
// PB1..5 (output) = digit cathodes
// PC0    (input)  = photoresistor
// PD0..7 (output) = segment anodes

// still need:
// motor enable (PB0?)
// xtal (PB6..7)
// stepper (PC1..4)
// two switches (PC5, ???)
// hmm, probably put the switches on PB0..1, motor enable on PC5, 
// ... move the colon to PD7, and lose the decimal points ???

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "util.h"
#include "display.h"

void adc_init()
{
	// power on the ADC
	PRR &= ~(1 << PRADC);
	
	// select AVCC reference, ADC0 source
	ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR);
	
	// enable ADC and start conversions at 1/128 prescaler
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

ISR(ADC_vect)
{
	// average a whole bunch of readings for the display
	static uint32_t accum = 0;
	static uint16_t count = 0;
	
	accum += ADC;
	if (++count == 2048) {
		display_number(accum >> 11);
		count = 0;
		accum = 0;
	}
}

int main(void)
{
	display_init();
	adc_init();

	// Enable interrupts
	sei();
	
    for(;;)
	{
		Sleep(250);
 	}
}

