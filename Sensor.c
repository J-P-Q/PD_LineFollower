#include <xc.h>

#include <stdint.h>

void Sensor_init(void){
    OPTION_REG |= (1 << 7);    
    TRISB |= (0x07 << 3);    // Set RB3, RB4, RB5 as input

    return;
}

uint8_t Sensor_read(void){
    return (uint8_t)((PORTB >> 3) & 0x07); // left to right [2:0]
}
