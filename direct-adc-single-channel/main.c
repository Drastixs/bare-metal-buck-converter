#define __AVR_ATmega328P__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <util/delay.h>
#define TARGET_VOLTAGE 3.3
#define MAX_CURRENT_MA 200
#define DUTY OCR0B
#define TOP 127

//adc voltage feedback on PC0 (ADC0)
//adc current feedback on PC1 (ADC1)
//linear regression equation for flowing current vs omp amp output y = 2.11415x + 0.235514
//pwm on OC0B (PD5)
uint16_t target_adc = TARGET_VOLTAGE/5 * 1023;
uint16_t max_shunt_adc = 200;//(2.11415* (MAX_CURRENT_MA/1000) + 0.235514)/5 * 1023;


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

    OCR0A = TOP;  // TOP value -> sets PWM frequency
    OCR0B = 63;   // Compare value -> sets duty cycle (8/31 = ~26%)


    //setup adc
    DDRC &= ~(1 << DDC0);//setup PC0 and PC1 as input pin

    //setup adc
    ADCSRA |= (1 << ADEN) | 0b111; //sets ADPS[2:0]. Enables adc and set clock prescaler as highest for greatest accuracy
    ADMUX |= (1 << REFS0) | 0b0000;//sets MUX[3:0]. sets internal reference voltage and sets ADC0 channel
    uint16_t adcResult;
    
    while (1){
        setVoltageFeedbackADC();
        adcResult = readADC();
        if (adcResult < target_adc){
            setDuty(DUTY +1);
        }
        else if(adcResult > target_adc){
            setDuty(DUTY -1);
        }
        setShuntCurrentADC();
        adcResult = readADC();
        if (adcResult > max_shunt_adc){
            PORTB |= (1 << PB1);
        }
        else {
            PORTB &= ~(1 << PB1);
        }
    }
}