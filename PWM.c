#include <xc.h>
#include <stdint.h>

void PWM_init(void){
    TRISC &= ~(3 << 2);

    CCP1CON = 0x0C;
    CCP2CON = 0x0C;

    PR2 = 0xFF; 

    T2CON |= (1 << 2);
    //T2CON [1:0] ---> 00 = 1, 01 = 4, 1X = 16 Figure out prescaler later

    return;
}

void PWM1_duty(uint16_t duty){
    CCPR1L = (uint8_t)(duty >> 2);
    CCP1CON = (duty & 0x03) << 4;
    return
}


void PWM2_duty(uint16_t duty){
    CCPR2L = (uint8_t)(duty >> 2);
    CCP2CON = (duty & 0x03) << 4;
    return;
}