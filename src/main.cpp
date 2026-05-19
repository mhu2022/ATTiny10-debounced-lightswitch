
#include <util/delay.h> // Requires that F_CPU is defined in the Makefile

int main(void) {
    while(1) {
        _delay_ms(1000);
    }
}