#pragma once

extern const uint8_t digits[10] PROGMEM;

#define DIGITS_OFF()   { PORTB |= 0x3e; }
#define DIGIT_ON(x)    { PORTB &= (2 << x) ^ 0x3e; }

void display_init();
void display_number(uint16_t num);
void display_colon(uint8_t on);

extern volatile uint8_t display[5];

