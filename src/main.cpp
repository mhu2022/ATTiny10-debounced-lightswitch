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

static inline void setup() {
    cli(); // Disable global interrupts during setup


    sei(); // Enable global interrupts
}

int main(void) {

    setup();

    while (1)
    {

        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
        sleep_mode(); 
    }
    
}
