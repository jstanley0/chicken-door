// chicken coop door opener firmware
// built around AVR ATMEGAxx8
// build with avr-gcc
//
// Source code (c) 2007-2016 by Jeremy Stanley
// Licensed under GNU GPL v2 or later

// Port assignments:
// PB0    (input)  = enter button
// PB1..5 (output) = display digit cathodes
// PC0    (input)  = photoresistor
// PC1    (input)  = select button
// PC2..5 (output) = stepper motor
// PD0..7 (output) = display segment anodes

// still need:
// xtal (PB6..7)
// motor enable?? might have to steal the colon or decimal point port for this

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "util.h"
#include "display.h"
#include "stepper.h"

void adc_init()
{
    // enable pull-up resistor on ADC input
    DDRC  &= 0b11111110;
    PORTC |= 0b00000001; 

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
	stepper_init();

	// Enable interrupts
	sei();
	
	ButtonState selectButton, enterButton;
	InitButtons(&selectButton, &PINB, &PORTB, &DDRB, 0b00000001);
	InitButtons(&enterButton, &PINC, &PORTC, &DDRC, 0b00000010);
	
    uint8_t motor_speed = 0;
    uint8_t disp = 1;
	for(;;)
	{
        if (GetButtons(&selectButton)) {
            motor_speed = motor_speed ? 0 : 96;
            if (motor_speed)
                stepper_enable();
            else
                stepper_off();
        }
        if (GetButtons(&enterButton)) {
            if (disp) {
                display_off();
            } else {
                display_on();
            }
            disp = !disp;
        }
	
	    if (motor_speed > 0) {
	        step_cw();
	        Sleep_kc(1000 - 10 * motor_speed);
	    } else {
	        Sleep(50);
	    }
 	}
}
