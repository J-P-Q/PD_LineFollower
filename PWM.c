#include <xc.h>

void PWM_init(void){
    TRISC &= ~(3 << 2);

    CCP1CON = 0x0C;
    CCP2CON = 0x0C;

    PR2 = 0xFF;
    PR2 = 0xFF;

    T2CON |= (1 << 2);
    //T2CON [1:0] ---> 00 = 1, 01 = 4, 1X = 16 Figure out prescaler later
}