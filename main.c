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
/* k_p history
#1 100 = undershoot
#2 200 = seems fine but on 3rd consecutive right angle, it (try changing base duty lower and max to 800 but k_p still 200)
#3 200 = maxDuty 700 -->800; baseDuty 300 --> 400 | still strays on 3rd right turn (try 850 max)
#4 200 = maxDuty 800; try 300 base duty 
STOP at 200, it wobbles so maybe it misses the line, stablizing with k_d might make readings more reliable at 3rd turn
------- using 5ms sensor reading interval
#5 400 = 200 k_p with 400 k_d still undershooting
#6 500 = i think worse case error is maxed out, but 1 and -1 errors should respond a little more to not miss turns

------ using errorPrev to follow through with turn (so far it run well with k_p 500 and k_d 400, but still wobbles)
#7 400 = try lowering assuming memory solution handles 90 degree turns

--- New trial, reset to 0
#1 20  (somehow works really well, maybe cause of memory solution, doesnt even need a k_d cause it doesnt wobble, but try upping the speed)
*/
#define k_i 0
#define k_d 0
/* k_d history
#1  50 = random assumption
#2 200 = 50 reduced wobble but not enough (still wobbles, maybe reduce baseDuty, cause k_p is at its max and still no response on 3rd turn)
#3 300 = reduced base duty from 400 t0 300, still wobbles but strays less on 3rd turn 
----- 300 does not miss turns but maybe a higher one will be more stable
#4 400 = 400 too unresponsive, maybe 350
------ using 5ms sensor reading interval

------ using errorPrev to follow through with turn (so far it run well with k_p 500 and k_d 400, but still wobbles)

-- new trial reset to 0 

*/

#define maxDuty 850 // 50% duty, cause motor will be overpowered by 8V batt
#define minDuty 200 // 100 just buzzes
#define baseDuty 300    // 




#include <Sensor.h>
#include <TMR0.h>
#include <PWM.h>

void PID(void);
void getError(uint8_t sensorReading);
void sanityTest(void);

volatile uint8_t sensorReading = 0;

enum sensorColor{
    // PID variables
    // Black = 1, White = 0
    WBW = 0b010,

    WBB = 0b011,
    WWB = 0b001,

    BBW = 0b110,
    BWW = 0b100,

    WWW = 0b000,
    BBB = 0b111,
    BWB = 0b101
};

volatile int32_t errorNow = 0;
volatile int32_t errorPrev = 0;
volatile int32_t errorSum = 0;
 
volatile uint8_t timeNow = 0;
volatile uint8_t timePrev = 0;

volatile uint8_t my10ms = 0;

// Moved outside to reduce PID() cycles 
volatile int32_t Product_k;
volatile int32_t Product_i;
volatile int32_t Product_d;

volatile int16_t finalDuty1;
volatile int16_t finalDuty2;

volatile uint8_t regularReading = 1;   // if 0, bot is probably off the line, so just follow through with turn


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
    PWM_init();
    
    timePrev = 0;
    timeNow = my10ms;

    PORTD = 0x00;

    while(1){
        timeNow = my10ms;
        
        if((uint8_t)(timeNow - timePrev) >= 1){   // VERY IMPORTANT (uint8_t)
            sensorReading = Sensor_read();
            PID();
            
            timePrev = timeNow;
        }
    }
    return;
}


void PID(void){ 
    // https://eng.libretexts.org/Bookshelves/Industrial_and_Systems_Engineering/Chemical_Process_Dynamics_and_Controls_(Woolf)/09%3A_Proportional-Integral-Derivative_(PID)_Control/9.02%3A_P_I_D_PI_PD_and_PID_control
    // https://apmonitor.com/pdc/index.php/Main/ProportionalIntegralDerivative
    getError(sensorReading);

    if(regularReading == 1){        
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


        PWM1_duty((uint16_t) finalDuty1);    // left motor
        PWM2_duty((uint16_t) finalDuty2);    // right motor
    }

    else if(regularReading == 0){  
        PWM1_duty(0);
        PWM2_duty(0);
        /*                      Memory Solution, but buggy
        if(errorPrev > 0){
            PWM1_duty(maxDuty);
            PWM2_duty(0);
        }
        else if(errorPrev < 0){
            PWM1_duty(0);
            PWM2_duty(maxDuty);
        }*/
    }
    errorPrev = errorNow;
    return;
}

void getError(uint8_t sensorReading){
    regularReading = 1;  
    switch(sensorReading){
            case WBW:
                errorNow = 0;
                break;

            // Turn Right    
            case WBB:
                errorNow = 1;
                break;
            case WWB:
                errorNow = 2;
                break;

            // turn left
            case BBW:
                errorNow = -1;
                break;
            case BWW:
                errorNow = -2;
                break;

            case WWW:
            case BBB:
            case BWB:
                regularReading = 0;  // indicate that the bot is off the line
                
                break;
            
            default:
                break;
    }
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