#define __AVR_ATmega328P__
#include <avr/io.h>
#define TOP OCR0A
#define DUTY OCR0B
//adc on PC0 and timer0 output on OCOA for fast pwm and adc output check on PB1

void startPWM(){
    TCCR0A |= (1 << COM0B1);//connect OC0B (PD5), non-inverting fast PWM
}

void stopPWM(){
    TCCR0A &= ~(1 << COM0B1);//disconnect OC0B from the timer
    PORTD &= ~(1 << PD5);//drive the pin low
}

void setDuty(uint8_t newDuty){
    if (newDuty > TOP){
        return;
    }//if DUTY == newDuty then nothing happens
    if (DUTY > 0 && newDuty == 0){
        stopPWM();
    }
    else if (DUTY == 0 && newDuty > 0){
        startPWM();
    }
    DUTY = newDuty;
}

void setVoltageFeedbackADC(){//set PC0 (ADC0) as adc channel
    ADMUX = (ADMUX & 0b11110000) | 0b0000;//changes MUX[3:0] = 0
}

void setShuntCurrentADC(){//set PC1 (ADC1) as adc channel
    ADMUX = (ADMUX & 0b11110000) | 0b0001;
}

uint16_t readADC(){
    ADCSRA |= (1 << ADSC);//start adc conversion
    while (ADCSRA >> ADSC & 1){}//wait for conversion to finish
    return ADC;
}

int main(void){
    //setup pwm
    DDRB |= (1 << DDB1);//PB1 as output
 // Set OC0B (PD5) as output
    DDRD |= (1 << DDD5);

    // Fast PWM, TOP = OCR0A (WGM02:0 = 0b111)
    TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00); // Non-inverting on OC0B
    TCCR0B = (1 << WGM02) | (1 << CS00); // No prescaler

    OCR0A = 127;  // TOP value -> sets PWM frequency
    OCR0B = 8;   // Compare value -> sets duty cycle (8/31 = ~26%)


    //setup adc
    DDRC &= ~(1 << DDC0) & ~(1 << DDC1);//setup PC0 as input pin

    //setup adc
    ADCSRA |= (1 << ADEN) | 0b111; //sets ADPS[2:0]. Enables adc and set clock prescaler as highest for greatest accuracy
    ADMUX |= (1 << REFS0) | 0b0000;//sets MUX[3:0]. sets internal reference voltage and sets ADC0 channel
    uint16_t adcResult;
    setVoltageFeedbackADC();
    while (1){
        adcResult = readADC();
        setDuty(adcResult >> 3);// adc 2^10 combos. PWM 2^7 combos
        if (adcResult > 512){
            PORTB |= (1 << PB1);//PB1 high
        }
        else{
            PORTB &= ~(1 << PB1);//PB1 low
        }
        
    }
}