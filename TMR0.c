#include <xc.h>

#define preloadCounter 195
#define timerPrescaler 0x07

void TMR0_init(void){   //100ms overflow
    
    // Tic/Sec = 20MHz / (4 * 256) = 19531.25
    // 1/19531.25 = 51.2us
    // 51.2us * 256 = 13.1072ms

    // 10ms / 51.2us = 195.3125 

    TMR0 = preloadCounter;
    OPTION_REG |= timerPrescaler; // prescaler = 256
    INTCON |= (1 << 5);
    return;
}

void TMR0_reset(void){
    TMR0 = preloadCounter;
    return;
}