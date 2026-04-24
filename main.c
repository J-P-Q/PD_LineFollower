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
void sanityTest(void);

volatile uint8_t sensorReading = 0;

// PID variables
// Black = 1, White = 0
volatile uint8_t errorTable[8] ={
    10,      // 000      (probably find line in this case)
    2,      // 001
    0,      // 010
    1,      // 011
    -2,     // 100
    10,     // 101      (highly unlikely)
    -1,     // 110
    10       // 111      (probably find line here)
};

volatile int32_t errorNow = 0;
volatile int32_t errorPrev = 0;
volatile int32_t errorSum = 0;
 
volatile uint8_t timeNow = 0;
volatile uint8_t timePrev = 0;

volatile uint8_t my10ms = 0;

void __interrupt() ISR(void){
    if(INTCON & 0x04){
        TMR0_reset();
        my10ms++;
        INTCON &= ~0x04;        
    }
    return;
}

void main(void) {
    // Clear everything
    OPTION_REG = 0x00;
    INTCON = 0x00;


    TRISD = 0x00;    // Set PORTD as output for debugging, remove after
    
    INTCON |= (1 << 7); //global int enable

    TMR0_init();
    Sensor_init();
    //PWM_init();
    

    timePrev = 0;
    timeNow = my10ms;

    PORTD = 0x00;


    while(1){
        timeNow = my10ms;
        
        if((uint8_t)(timeNow - timePrev) >= 100){   // VERY IMPORTANT (uint8_t)
            sensorReading = Sensor_read();
            
            
            timePrev = timeNow;
        }
        // This was old code, uncomment after test
        /*
        timeNow = my10ms;
        if(timeNow - timePrev >= 1){   // 10ms has passed
            sensorReading = Sensor_read();
            PID();
            timePrev = timeNow;
        }*/
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
    if(errorNow == 10){
        finalDuty1 = 0;
        finalDuty2 = 0;
    }
    else{
        errorSum = errorSum + errorNow;
        
        Product_k = k_p * errorNow;
        Product_i = k_i * errorSum;
        Product_d = k_d * (float)(errorNow - errorPrev);

        finalDuty1 = (int16_t) (baseDuty + Product_k + Product_i + Product_d);
        finalDuty2 = (int16_t) (baseDuty - Product_k - Product_i - Product_d);

        if(finalDuty1 > maxDuty){
            finalDuty1 = maxDuty;
        }
        else if(finalDuty1 < minDuty){
            finalDuty1 = 0;
        }

        if(finalDuty2 > maxDuty){
            finalDuty2 = maxDuty;
        }
        else if(finalDuty2 < minDuty){
            finalDuty2 = 0;
        }
    }

    PWM1_duty((uint16_t) finalDuty1);    // Right motor
    PWM2_duty((uint16_t) finalDuty2);    // Left motor

    errorPrev = errorNow;
    return;
}

void sanityTest(void){
    if      (sensorReading == 0x02){
        PWM1_duty(600);
        PWM2_duty(600);
    }
    else if (sensorReading == 0x01){
        PWM1_duty(0);
        PWM2_duty(700);
    }
    else if (sensorReading == 0x04){
        PWM1_duty(700);
        PWM2_duty(0);
    }
    return;
}