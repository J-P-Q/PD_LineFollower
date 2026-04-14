#include <xc.h>

void TMR0_init(void){   //100ms overflow
    
    // Tic/Sec = 20MHz / (4 * 256) = 19531.25
    // Tic/100ms = 19531.25 / 100 = 195.3125
    // 256 - 195.3125 = 60.6875 ~ 61
    TMR0 = 61;
    OPTION_REG |= 4; // prescaler = 32
    INTCON |= (1 << 5);
    return;
}

void TMR0_reset(void){
    TMR0 = 61;
    return;
}