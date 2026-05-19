#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define OUTPUT_PIN PB0
#define BUTTON_PIN PB2
#define BUTTON_PUE PUEB2

#define FLAG_STATE_DEBOUNCING 0x01
#define FLAG_TRIGGER_TOGGLELIGHT 0x02

volatile uint8_t gpFlags;
volatile uint8_t buttonBounceHistory = 0x00;

static inline void testThroughLed() {
    // change the state of the output pin to indicate an error (e.g., in BADISR_vect)
    if(PORTB & (1 << OUTPUT_PIN)) {
        PORTB &= ~(1 << OUTPUT_PIN); // this is indeed working, if the led comes off.
        PORTB |= (1 << OUTPUT_PIN); // this is indeed working, if the led comes on.
    }
    else {
        PORTB |= (1 << OUTPUT_PIN); // this is indeed working, if the led comes on.
        PORTB &= ~(1 << OUTPUT_PIN); // this is indeed working, if the led comes off.
    }
}

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

static inline void useTimerInterrupt0At500usInterval() {
    TCCR0A = (1 << WGM01);                              // Set CTC Mode (WGM0 = 2)
    //OCR0A = 78;                                         // Set Compare Match value 78 * 64 prescaler / 1MHz = ~5ms
    //OCR0A = 15;                                         // Set Compare Match value 15 * 64 prescaler / 1MHz = ~1ms
    OCR0A = 75;                                         // Set Compare Match value 75 * 8 prescaler / 1MHz = ~500us
    TIMSK0 |= (1 << OCIE0A);                            // Enable Output Compare A Match Interrupt
    //TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);  // Start the timer with a prescaler of 64.
    TCCR0B = (1 << WGM02) | (1 << CS01);  // Start the timer with a prescaler of 8.
}

static inline void setup() {
    cli(); // Disable global interrupts during setup

    useOuputPin();

    useButtonInputPinWithPullup();

    useTimerInterrupt0At500usInterval();

    sei(); // Enable global interrupts
}

// Pin Change Interrupt - triggered on any change on PB2 (button)
ISR(PCINT0_vect) {
    gpFlags |= FLAG_STATE_DEBOUNCING;   // Set debouncing state flag
}

// Timer0 ISR: Executes in background every 500us to clean the signal
ISR(TIM0_COMPA_vect) {

    if(!(gpFlags & FLAG_STATE_DEBOUNCING)) {
        return; // If not in debouncing state, do nothing
    }

    uint8_t raw_sample = !((PINB >> BUTTON_PIN) & 0x01);     // Invert read because pull-up makes a press equal 0
    buttonBounceHistory = (buttonBounceHistory << 1) | raw_sample;

    // Detect exact moment button transitions to a stable pressed state (0b01111111)
    if (buttonBounceHistory == 0x7F) {
        gpFlags |= FLAG_TRIGGER_TOGGLELIGHT;     // Set button trigger flag
    }
    // Indien het signaal 8x2ms stabiel is (0x00 of 0xFF): stop de timer
    else if ((buttonBounceHistory == 0x00) || (buttonBounceHistory == 0xFF)) {
        buttonBounceHistory = 0x00;  // <-- Add this line to reset the history after stable state is reached
        gpFlags &= ~FLAG_STATE_DEBOUNCING;     // Clear button trigger flag
    }

}

// This is a catch-all for any unexpected interrupts, which should not happen in this program. If it does, we can use the LED to indicate an error state.
ISR(BADISR_vect, ISR_NAKED) {
    testThroughLed();
    testThroughLed();
    testThroughLed();
}

int main(void) {

    setup();

    while (1)
    {
        cli(); // make sure to disable interrupts while reading and clearing flags to prevent race conditions
        bool debouncingState = gpFlags & (FLAG_STATE_DEBOUNCING);
        gpFlags &= ~(FLAG_STATE_DEBOUNCING); // Clear debouncing state flag after reading
        sei();

        if(debouncingState) {
            set_sleep_mode(SLEEP_MODE_IDLE);
        }
        else {
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        }

        sleep_mode(); 
    }
    
}
