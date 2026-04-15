#include <xc.h>

#include <stdint.h>

void Sensor_init(void){
    TRISB |= 0x38;
    OPTION_REG |= (1 << 7);

    return;
}

uint8_t Sensor_read(void){
    return (uint8_t)((PORTB >> 3) & 0x07); // left to right [2:0]
}
