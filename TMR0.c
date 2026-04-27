#include <xc.h>

#define preloadCounter 158
#define timerPrescaler 0x07

void TMR0_init(void){   //100ms overflow
    
    // Tic/Sec = 20MHz / (4 * 256) = 19531.25
    // 1/19531.25 = 51.2us
    // 51.2us * 256 = 13.1072ms

    // Old calculations     (265 instead of 256, but it was close enough to 10ms)
    // 10ms / 51.2us = 195.3125 
    // 256 - 195.3125 = 60

    // New calculations     (try 5ms)
    // 5ms / 51.2us = 97.65625
    // 256 - 97.65625 = 158.34375 (rounded to 158)

    TMR0 = preloadCounter;
    OPTION_REG = (OPTION_REG & 0xF8) | timerPrescaler; // prescaler = 256
    INTCON |= (1 << 5);
    return;
}

void TMR0_reset(void){
    TMR0 = preloadCounter;
    return;
}