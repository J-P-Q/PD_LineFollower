#include <xc.h>
#include <stdint.h>

void PWM_init(void){
    /*
        OSC = 20MHz
        Prescaler 16
        TMR2 Period = 255
        PWM Freq = 1220Hz 
    */
    TRISC &= ~(3 << 1);

    CCP1CON = 0x0C;
    CCP2CON = 0x0C;

    PR2 = 0xFF; 

    T2CON |= (1 << 2);
    T2CON |= (1 << 1);      // Prescaler 16

    return;
}

void PWM1_duty(uint16_t duty){
    CCPR1L = (uint8_t)(duty >> 2);
    CCP1CON |= (uint8_t)((duty & 0x03) << 4);
    return;
}


void PWM2_duty(uint16_t duty){
    CCPR2L = (uint8_t)(duty >> 2);
    CCP2CON |= (uint8_t)((duty & 0x03) << 4);
    return;
}