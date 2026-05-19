#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define OUTPUT_PIN PB0
#define BUTTON_PIN PB2
#define BUTTON_PUE PUEB2

static inline void useOuputPin() {
    DDRB |= (1 << OUTPUT_PIN); // Set PB0 as output
    PORTB &= ~(1 << OUTPUT_PIN); // Ensure the output starts LOW
}

static inline void useButtonInputPinWithPullup() {
    DDRB &= ~(1 << BUTTON_PIN); // Set PB2 as input
    PUEB |= (1 << BUTTON_PUE);      // Pull-up voor de drukknop (PB2)

    // Enable pin change interrupt for PB2
    PCMSK |= (1 << PCINT2); // Enable pin change interrupt for PB2
    PCICR |= (1 << PCIE0);  // Enable pin change interrupts for the group that includes PB2
}

static inline void setup() {
    cli(); // Disable global interrupts during setup

    useOuputPin();

    useButtonInputPinWithPullup();

    sei(); // Enable global interrupts
}

// Pin Change Interrupt - triggered on any change on PB2 (button)
ISR(PCINT0_vect) {
}

int main(void) {

    setup();

    while (1)
    {

        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
        sleep_mode(); 
    }
    
}
