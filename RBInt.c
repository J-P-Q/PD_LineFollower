#include <xc.h>

void RBInt_init(void){
    TRISB |= 0x70;
    OPTION_REG |= (1 << 7);

    INTCON |= (1 << 7) | (1 << 3);

    return;
}

