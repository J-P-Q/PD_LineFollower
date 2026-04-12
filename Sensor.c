#include <xc.h>

#include <stdint.h>

uint8_t Sensor_read(void){
    return (uint8_t)((PORTB >> 4) & 0x07); // left to right [2:0]
}
