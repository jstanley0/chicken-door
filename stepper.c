#include <avr/io.h>
#include <avr/pgmspace.h>
#include "stepper.h"

#define ENABLE_PORT   PORTB
#define ENABLE_DDR    DDRB
#define ENABLE_MASK   0b11000000

#define STEPPER_PORT  PORTC
#define STEPPER_DDR   DDRC
#define STEPPER_START 2
#define STEPPER_MASK  0b00111100

#define STEPPER_OFF() { STEPPER_PORT &= ~STEPPER_MASK; }
#define ENABLE_OFF() { ENABLE_PORT &= ~ENABLE_MASK; }

const uint8_t stepper_bits[4] PROGMEM = {
  0b0101 << STEPPER_START,
  0b0110 << STEPPER_START,
  0b1010 << STEPPER_START,
  0b1001 << STEPPER_START
};

static uint8_t stepper_pos = 0;

void stepper_init()
{
    STEPPER_DDR |= STEPPER_MASK;
    ENABLE_DDR |= ENABLE_MASK;
    STEPPER_OFF();
    ENABLE_OFF();
}

void stepper_enable()
{
    ENABLE_PORT |= ENABLE_MASK;
}

void pulse()
{
    STEPPER_OFF();
    STEPPER_PORT |= pgm_read_byte(&stepper_bits[stepper_pos]);
}

void step_cw()
{
    stepper_pos = (stepper_pos - 1) & 3;
    pulse();
}

void step_ccw()
{
    stepper_pos = (stepper_pos + 1) & 3;
    pulse();
}

void stepper_off()
{
    STEPPER_OFF();
    ENABLE_OFF();
}
