#include "math_utils.h"
uint8_t min(uint8_t num1, uint8_t num2){
    if (num1 > num2){
        return num2;
    }
    return num1;
}

uint8_t max(uint8_t num1, uint8_t num2){
    if (num1 > num2){
        return num1;
    }
    return num2;
}

short abs(short num){
    if (num < 0){
        return num * -1;
    }
    return num;
}