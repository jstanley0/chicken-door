#pragma once

#define BUTTON_HOLD   0x100 	// button was held
#define REPEAT_THRESHOLD 15

void Sleep_kc(uint16_t kiloclocks);
// note: use constants, so the compiler does this math, not the poor AVR
#define Sleep(ms) Sleep_kc((int)((ms) * (F_CPU / 1024000.0)))

typedef struct {
    volatile uint8_t *pin;
    uint8_t mask;
	uint8_t prevState;
	uint8_t repeat;
} ButtonState;

void InitButtons(ButtonState *state, volatile uint8_t *pin, volatile uint8_t *port,
                 volatile uint8_t *ddr, uint8_t mask);

// poll this function every 50ms or so.
// here's how button presses work:
// - a press is registered when a button is released.
// - a hold is registered when the same button has been down
//   for a specified number of cycles.  the button release
//   following the hold does not register.
// returns the bit(s) corresponding to buttons pressed,
// OR'd with BUTTON_HOLD if a button was held down.
uint16_t GetButtons(ButtonState *state);
