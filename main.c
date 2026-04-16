/*
 * File:   main.c
 * Author: Johnn
 *
 * Created on April 12, 2026, 7:55 PM
 */


#include <xc.h>

#pragma config FOSC = HS        
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config BOREN = OFF     
#pragma config LVP = OFF        
#pragma config CPD = OFF       
#pragma config WRT = OFF       
#pragma config CP = OFF  

#include <stdint.h>

#define _XTAL_FREQ 20000000 

#define k_p 20
#define k_i 1
#define k_d 1

#define maxDuty 511 // 50% duty, cause motor will be overpowered by 8V batt
#define baseDuty 255    //50% of max = 511



#include <Sensor.h>
#include <TMR0.h>
#include <PWM.h>

void PID(void);

uint8_t sensorReading = 0;

// PID variables
// Black = 1, White = 0
int8_t errorTable[8] ={
    0,      // 000      (probably find line in this case)
    2,      // 001
    0,      // 010
    1,      // 011
    -2,     // 100
    0,      // 101      (highly unlikely)
    -1,     // 110
    0       // 111      (probably find line here)
};

int32_t errorNow = 0;
int32_t errorPrev = 0;
int32_t errorSum = 0;

uint8_t timeNow = 0;
uint8_t timePrev = 0;

void __interrupt() ISR(void){
    if(INTCON & 0x04){
        TMR0_reset();
        sensorReading = Sensor_read();
        INTCON &= ~0x04;
    }
    return;
}

void main(void) {

    Sensor_init();
    PWM_init();
    //TMR0_init();
   
    
    //---TESTING ISR
    
    //--------------

    while(1){
        //---TESTING
       
        PWM1_duty(0);
        PWM2_duty(0);
        __delay_ms(2000);
        PWM1_duty(100);
        __delay_ms(2000);
        PWM1_duty(200);
        __delay_ms(2000);
        PWM1_duty(400);
        __delay_ms(2000);
        PWM1_duty(800);   
        __delay_ms(2000);

        
        PWM2_duty(100);
        __delay_ms(2000);
        PWM2_duty(200);
        __delay_ms(2000);
        PWM2_duty(400);
        __delay_ms(2000);
        PWM2_duty(800);   
        __delay_ms(2000);
    }
    return;
}

// https://eng.libretexts.org/Bookshelves/Industrial_and_Systems_Engineering/Chemical_Process_Dynamics_and_Controls_(Woolf)/09%3A_Proportional-Integral-Derivative_(PID)_Control/9.02%3A_P_I_D_PI_PD_and_PID_control
// https://apmonitor.com/pdc/index.php/Main/ProportionalIntegralDerivative
void PID(void){ 
    int32_t Product_k;
    int32_t Product_i;
    int32_t Product_d;

    errorNow = errorTable[sensorReading];
    timeNow =  TMR0;
    errorSum = errorSum + errorTable[sensorReading];
    
    Product_k = k_p * errorNow;
    Product_i = k_i * errorSum;
    Product_d = k_d * ((float)(errorNow - errorPrev)/((float)(timeNow - timePrev)));

    PWM1_duty(maxDuty & (uint16_t) (baseDuty + Product_k + Product_i + Product_d));    // Right motor
    PWM2_duty(maxDuty & (uint16_t) (baseDuty - Product_k - Product_i - Product_d));    // Left motor

    errorPrev = errorNow;
    timePrev = timeNow;
    return;
}
