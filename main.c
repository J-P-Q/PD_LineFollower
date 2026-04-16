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

#define k_p 100
#define k_i 0
#define k_d 0

#define maxDuty 700 // 50% duty, cause motor will be overpowered by 8V batt
#define minDuty 200 // 100 just buzzes
#define baseDuty 450    // (700 + 200)/2 = 450




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
        //sensorReading = Sensor_read();
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
        sensorReading = 0x02;
        PID();
        __delay_ms(1000);

        PWM1_duty(0);
        PWM2_duty(0);
        __delay_ms(1000);

        sensorReading = 0x01;
        PID();
        __delay_ms(1000);

        PWM1_duty(0);
        PWM2_duty(0);
        __delay_ms(1000);


        sensorReading = 0x04;
        PID();
        __delay_ms(1000);
    
        PWM1_duty(0);
        PWM2_duty(0);
        __delay_ms(1000);
    }
    return;
}

// https://eng.libretexts.org/Bookshelves/Industrial_and_Systems_Engineering/Chemical_Process_Dynamics_and_Controls_(Woolf)/09%3A_Proportional-Integral-Derivative_(PID)_Control/9.02%3A_P_I_D_PI_PD_and_PID_control
// https://apmonitor.com/pdc/index.php/Main/ProportionalIntegralDerivative
void PID(void){ 
    int32_t Product_k;
    int32_t Product_i;
    int32_t Product_d;

    uint16_t finalDuty1;
    uint16_t finalDuty2;

    errorNow = errorTable[sensorReading];
    timeNow =  TMR0;
    errorSum = errorSum + errorTable[sensorReading];
    
    Product_k = k_p * errorNow;
    Product_i = k_i * errorSum;
    Product_d = k_d * ((float)(errorNow - errorPrev)/((float)(timeNow - timePrev)));

    finalDuty1 = (int16_t) (baseDuty + Product_k + Product_i + Product_d);
    finalDuty2 = (int16_t) (baseDuty - Product_k - Product_i - Product_d);

    if(finalDuty1 > maxDuty){
        finalDuty1 = maxDuty;
    }
    else if(finalDuty1 < minDuty){
        finalDuty1 = minDuty;
    }

    if(finalDuty2 > maxDuty){
        finalDuty2 = maxDuty;
    }
    else if(finalDuty2 < minDuty){
        finalDuty2 = minDuty;
    }

    PWM1_duty((uint16_t) finalDuty1);    // Right motor
    PWM2_duty((uint16_t) finalDuty2);    // Left motor

    errorPrev = errorNow;
    timePrev = timeNow;
    return;
}
