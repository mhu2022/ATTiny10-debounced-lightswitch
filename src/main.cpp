// ATTINY10 Debounced Light Switch
// This code implements a debounced light switch using an ATTINY10 microcontroller. It detects button presses and long presses, toggling an output pin accordingly. The code uses pin change interrupts for button state changes and a timer interrupt for debouncing and long press detection.
// Running at 1 MHz internal clock, with a 1ms timer interval for debouncing and long press timing.
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define OUTPUT_PIN PB0
#define BUTTON_PIN PB2
#define BUTTON_PUE PUEB2

#define FLAG_STATE_DEBOUNCING 0x01
#define FLAG_STATE_LONGPRESS 0x04

#define FLAG_TRIGGER_TOGGLELIGHT 0x10   // Tells the main loop to toggle the light state on the next iteration
#define FLAG_TRIGGER_LONGPRESS 0x20     // Tells the main loop to execute the long press action on the next iteration

#define RISING_EDGE 0x7FU
#define FALLING_EDGE 0xFEU
#define STABLE_LOW 0x00U
#define STABLE_HIGH 0xFFU

#define DEBOUNCE_MIN_TICKS 20U      // Minimum ticks to consider a valid press, this is used to filter out very short presses that are likely just noise. At 1ms intervals, 20 ticks would be 20ms, which is a common debounce time.
#define LONG_PRESS_TICKS 4000U      // 4 seconds at 1ms intervals

volatile uint8_t gpFlags;
volatile uint8_t buttonBounceHistory = 0x00;
volatile uint16_t longPressCounter = 0;

static inline void toggleOutputPin() {
    PORTB ^= (1 << OUTPUT_PIN); // Toggle the output pin state
}

static inline void testThroughLed() {
    // change the state of the output pin to indicate an error (e.g., in BADISR_vect)
    toggleOutputPin();
    _delay_ms(50); // Short delay to make the toggle visible
}

static inline void testThroughLedx10() {
    for(uint8_t i = 0; i < 10; i++) {
        testThroughLed();
    }
}

static inline void useOuputPin() {
    DDRB |= (1 << OUTPUT_PIN); // Set PB0 as output
    PORTB &= ~(1 << OUTPUT_PIN); // Ensure the output starts LOW
}

static inline void useButtonInputPinWithPullupAndInterrupt() {
    DDRB &= ~(1 << BUTTON_PIN); // Set PB2 as input
    PUEB |= (1 << BUTTON_PUE);      // Pull-up voor de drukknop (PB2)

    // Enable pin change interrupt for PB2
    PCMSK |= (1 << PCINT2); // Enable pin change interrupt for PB2
    PCICR |= (1 << PCIE0);  // Enable pin change interrupts for the group that includes PB2
}

static inline void useTimerInterrupt0At1msInterval() {
    TCCR0A = 0;                          // WGM00 = 0, WGM01 = 0 (Normale poortwerking)
    OCR0A = 124;                         // 125 ticks bij een 8-prescaler op 1MHz = exact 1,00 ms
    TIMSK0 |= (1 << OCIE0A);             // Schakel Output Compare A Match Interrupt in
    TCCR0B = (1 << WGM02) | (1 << CS01); // Mode 4: CTC (WGM02=1), Prescaler = 8 (CS01=1)
}

static inline void stopTimer() {
    TIMSK0 &= ~(1 << OCIE0A);   // Stop the timer to prevent further interrupts during the long press action
}

static inline void resumeTimer() {
    TIMSK0 |= (1 << OCIE0A);    // Herstel timer interrupts en reset de debounce status
}

static inline void resetStateMachine() {
    buttonBounceHistory = 0x00;
    longPressCounter = 0;
    gpFlags &= ~(FLAG_STATE_LONGPRESS|FLAG_STATE_DEBOUNCING);
    PCIFR |= (1 << PCIF0);  // Clear any pending 'rebound' interrupts in the hardware buffer
}

static inline void setup() {
    cli(); // Disable global interrupts during setup

    useOuputPin();

    useButtonInputPinWithPullupAndInterrupt();

    useTimerInterrupt0At1msInterval();

    sei(); // Enable global interrupts
}

// 1. Wakes up the MCU from Power Down when the button is first touched
ISR(PCINT0_vect) {
    gpFlags |= FLAG_STATE_DEBOUNCING; 
}

// 2. Ticks every 1ms when awake to track the button state
ISR(TIM0_COMPA_vect) {
    uint8_t currentPinState = !((PINB >> BUTTON_PIN) & 0x01);
    buttonBounceHistory = (buttonBounceHistory << 1) | currentPinState;

    // 1. EDGE DETECTION (Debounced)
    if (buttonBounceHistory == 0x7F) { 
        // Button was cleanly pressed down (Transitioned from 0s to stable 1s)
        longPressCounter = 0;
        gpFlags &= ~FLAG_STATE_LONGPRESS; // Clear long press state in case it was set from a previous press
    } 

    // 2. Button held down:
    if((buttonBounceHistory & 0x0F) == 0x0F) {
        if (!(gpFlags & FLAG_STATE_LONGPRESS)) {
            longPressCounter++;
            
            //if ((longPressCounter & 0xFF) == 0) { PORTB ^= (1 << OUTPUT_PIN); } test code to see the counter in action, toggling the output pin every 256ms while the button is held down, this can be useful for debugging long press detection without having to wait the full 4 seconds.

            // Check if threshold reached
            if (longPressCounter >= LONG_PRESS_TICKS) {
                gpFlags |= FLAG_TRIGGER_LONGPRESS;
                gpFlags |= FLAG_STATE_LONGPRESS; // Mark as handled to suppress short-press on release
            }
        }
    }
    // Button released:
    if((buttonBounceHistory & 0x0F) == 0x00) {
        if (!(gpFlags & FLAG_STATE_LONGPRESS) && (longPressCounter > DEBOUNCE_MIN_TICKS)) {
            gpFlags |= FLAG_TRIGGER_TOGGLELIGHT;
        }
        // Reset counters and sequence flags
        longPressCounter = 0;
        gpFlags &= ~(FLAG_STATE_LONGPRESS);
        gpFlags &= ~FLAG_STATE_DEBOUNCING;
        
        PCIFR |= (1 << PCIF0); // Clear residual contact bounces
    }

}

// This is a catch-all for any unexpected interrupts, which should not happen in this program. If it does, we can use the LED to indicate an error state.
// ISR(BADISR_vect, ISR_NAKED) {
//     testThroughLed(3);
// }

int main(void) {

    setup();

    while (1)
    {
        cli(); // make sure to disable interrupts while reading and clearing flags to prevent race conditions
        
        bool debouncingState = gpFlags & (FLAG_STATE_DEBOUNCING);
        bool triggeredToggleLight = gpFlags & (FLAG_TRIGGER_TOGGLELIGHT);
        bool triggeredLongPress = gpFlags & (FLAG_TRIGGER_LONGPRESS);
   
        if(triggeredToggleLight) {
            toggleOutputPin();
            gpFlags &= ~(FLAG_TRIGGER_TOGGLELIGHT); 
        }

        if(triggeredLongPress) {
            stopTimer(); // Stop the timer to prevent further interrupts during the long press action
            testThroughLedx10(); // Indicate long press with a second of LED blink.
            resumeTimer();
            resetStateMachine(); // Reset the state machine to be ready for the next button press sequence, this is necessary to prevent the long press trigger from firing repeatedly if the button is held down after a long press has been registered
            debouncingState = false;
            gpFlags &= ~(FLAG_TRIGGER_LONGPRESS); 
        }

        if(debouncingState) {
            set_sleep_mode(SLEEP_MODE_IDLE);
        }
        else {
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        }

        sleep_enable();
        sei();         // Interrupts are guaranteed to only execute AFTER the sleep instruction
        sleep_cpu();   // Safe sleep window
        sleep_disable(); 
    }
    
}
