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

#define k_p 1
#define k_i 1
#define k_d 1

#include <RBInt.h>
#include <Sensor.h>
#include <TMR0.h>

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
    if(INTCON & 0x01){
        sensorReading = Sensor_read();
        INTCON &= ~0x01;
    }

    if(INTCON & 0x04){
        TMR0_reset();
        INTCON &= ~0x04;
    }
    return;
}

void main(void) {

    RBInt_init();
    TMR0_init();
    
    //---TESTING ISR
    TRISD = 0x00;
    PORTD = 0x00;
    //--------------

    while(1){
        //---TESTING ISR
        PORTD = sensorReading;
        //--------------
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

    errorPrev = errorNow;
    timePrev = timeNow;
    return;
}
