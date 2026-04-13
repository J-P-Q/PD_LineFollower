/*
 * File:   main.c
 * Author: Johnn
 *
 * Created on April 12, 2026, 7:55 PM
 */


#include <xc.h>
#define _XTAL_FREQ 20000000 

#include <RBInt.h>
#include <Sensor.h>

uint8_t sensorReading = 0;

void __interrupt() ISR(void){
    if(INTCON & 0x01){
        sensorReading = Sensor_read();
        INTCON &= ~0x01;
    }
    return;
}

void main(void) {

    while(1){


    }
    return;
}
