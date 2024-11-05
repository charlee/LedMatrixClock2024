#ifndef __FONT5X7_H__
#define __FONT5X7_H__

#include <Arduino.h>

#define DIGIT_SPACE 10
#define DIGIT_E 11

const uint8_t FONT5x7[] = {
    // 0
    0b01110,
    0b10001,
    0b10011,
    0b10101,
    0b11001,
    0b10001,
    0b01110,
    
    // 1
    0b00100,
    0b01100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b01110,

    // 2
    0b01110,
    0b10001,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b11111,
    
    // 3
    0b11111,
    0b00010,
    0b00100,
    0b00010,
    0b00001,
    0b10001,
    0b01110,
    
    // 4
    0b00010,
    0b00110,
    0b01010,
    0b10010,
    0b11111,
    0b00010,
    0b00010,
    
    // 5
    0b11111,
    0b10000,
    0b11110,
    0b00001,
    0b00001,
    0b10001,
    0b01110,
    
    // 6
    0b00110,
    0b01000,
    0b10000,
    0b11110,
    0b10001,
    0b10001,
    0b01110,
    
    // 7
    0b11111,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b01000,
    0b01000,
    
    // 8
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b10001,
    0b10001,
    0b01110,

    // 9
    0b01110,
    0b10001,
    0b10001,
    0b01111,
    0b00001,
    0b00010,
    0b01100,
    
    // ' '
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,

    // 'E'
    0b11111,
    0b10000,
    0b10000,
    0b11100,
    0b10000,
    0b10000,
    0b11111,

    // '-'
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b00000,
    0b00000,
    0b00000,
};

const uint8_t COLON[] = {
    0b00,
    0b11,
    0b11,
    0b00,
    0b11,
    0b11,
    0b00,
};

#endif