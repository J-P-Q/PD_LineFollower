/*
 * File:   main.c
 * Author: Johnn
 *
 * Created on April 12, 2026, 7:55 PM
 */


#include <xc.h>
#define _XTAL_FREQ 20000000 

#define k_p 1
#define k_i 1
#define k_d 1

#include <RBInt.h>
#include <Sensor.h>

uint8_t sensorReading = 0;

// PID variables
// Black = 1, White = 0
uint8_t errorTable[8] ={
    0,      // 000      (probably find line in this case)
    2,      // 001
    0,      // 010
    1,      // 011
    -2,     // 100
    0,      // 101      (highly unlikely)
    -1,     // 110
    0       // 111      (probably find line here)
};

uint64_t errorNow = 0;
uint64_t errorPrev = 0;
uint64_t errorSum = 0;

void __interrupt() ISR(void){
    if(INTCON & 0x01){
        sensorReading = Sensor_read();
        INTCON &= ~0x01;
    }
    return;
}

void main(void) {

    RBInt_init();


    while(1){
    

    }
    return;
}

// https://eng.libretexts.org/Bookshelves/Industrial_and_Systems_Engineering/Chemical_Process_Dynamics_and_Controls_(Woolf)/09%3A_Proportional-Integral-Derivative_(PID)_Control/9.02%3A_P_I_D_PI_PD_and_PID_control
// https://apmonitor.com/pdc/index.php/Main/ProportionalIntegralDerivative
void PID(){ 
    errorNow = errorTable[sensorReading];
    errorSum = errorSum + errorTable[sensorReading];

    Product_k = k_p * errorNow;
    Product_i = k_i * errorSum;
    Product_d = k_d * (errorNow - errorPrev);

    errorPrev = errorNow;
}
