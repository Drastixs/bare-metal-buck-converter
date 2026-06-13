#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

// Dual-channel bare-metal buck-boost converter on a single ATmega328P.
//
// One source, two builds via a compile flag:
//   - control only          : drop -DWITH_DISPLAY from the Makefile
//   - control + OLED readout : compile with -DWITH_DISPLAY (default here)
//
// The control firmware is identical either way. Defining WITH_DISPLAY adds a
// per-channel voltage / current / power readout on a 128x32 SSD1306 OLED, drawn
// with the from-scratch overlay-block graphics library in ../display-lib/.
// Without the flag, all display code is #ifdef-ed out and display-lib isn't
// needed.
//
// Two independent closed-loop channels. Each channel:
//   - generates a ~62.5 kHz fast-PWM gate drive (TOP = 127, no prescaler)
//   - reads its output voltage on an ADC channel  (feedback)
//   - reads its shunt current on an ADC channel    (0.1 ohm, via LM324)
//   - reads a potentiometer on an ADC channel       (user voltage setpoint)
//   - drives an over-current flag pin high when the shunt current exceeds the limit
//
// Pin / channel map (control):
//                       PWM out        V-fb     I-fb     pot       OC flag
//   Channel 1   Timer0  OC0B (PD5)     ADC0     ADC1     ADC2      PB0
//   Channel 2   Timer2  OC2B (PD3)     ADC3     ADC4     ADC5      PB1
//   Display (WITH_DISPLAY): SSD1306 over I2C (SDA=PC4/A4, SCL=PC5/A5) -- NOTE
//   this shares the ADC port pins; on real hardware put the OLED on the TWI
//   pins and the analog feedback on the remaining ADC channels accordingly.

#ifdef WITH_DISPLAY
// SSD1306 overlay-block graphics library (../display-lib)
#include "driver-with-overlay-blocks/i2c.h"
#include "driver-with-overlay-blocks/canvas.h"
#endif

#define TOP             127
#define MAX_SHUNT_ADC   200       // over-current trip in raw ADC counts

#ifdef WITH_DISPLAY
#define DISPLAY_EVERY   2000      // refresh the OLED once per N control loops

// ---- Calibration constants (measured on the bench) ----
// These are derived from my own test measurements/calculations rather than
// nominal part values, to keep the readout as accurate as possible. Keep them
// unless you re-characterise your own divider / shunt / op-amp.
//
// Output-voltage feedback: ADC count -> millivolts at the converter output.
// vout_mV = adc_v * VOUT_MV_NUM / VOUT_MV_DEN
// (2:1 divider on a 5.0 V (AVcc) reference: full-scale 1023 -> 10000 mV)
#define VOUT_MV_NUM     10000L
#define VOUT_MV_DEN     1023L
// Shunt current from the LM324 output, inverting the measured linear fit
//   y = 2.11415*x + 0.235514   (x = current in A, y = op-amp output in V)
// (line of best fit from bench test calculations, hence the extra precision)
// so   I_mA = (Vopamp_mV - 235.514) / 2.11415   (Vopamp_mV = adc_i*5000/1023)
#define IOPAMP_MV(adc)  ((long)(adc) * 5000L / 1023L)
#define IOFFSET_MV      236L      // 0.235514 V intercept, in mV
#define ISLOPE_MV_PER_A 2114L     // 2.11415 V/A, in mV/A
#endif

// ---- PWM start/stop, per timer (connect/disconnect the OCnB pin) ----
static void ch1_startPWM(void){ TCCR0A |=  (1 << COM0B1); }
static void ch1_stopPWM(void) { TCCR0A &= ~(1 << COM0B1); PORTD &= ~(1 << PD5); }
static void ch2_startPWM(void){ TCCR2A |=  (1 << COM2B1); }
static void ch2_stopPWM(void) { TCCR2A &= ~(1 << COM2B1); PORTD &= ~(1 << PD3); }

struct Channel {
    volatile uint8_t *duty_reg;     // OCR0B / OCR2B
    uint8_t           v_adc;        // ADC channel: output voltage feedback
    uint8_t           i_adc;        // ADC channel: shunt current feedback
    uint8_t           pot_adc;      // ADC channel: setpoint potentiometer
    volatile uint8_t *flag_port;    // port holding the over-current flag pin
    uint8_t           flag_pin;     // bit of the over-current flag pin
    void            (*startPWM)(void);
    void            (*stopPWM)(void);
    uint16_t          last_v_adc;   // cached readings (used by the display build)
    uint16_t          last_i_adc;
};

static Channel ch[2] = {
    { &OCR0B, 0, 1, 2, &PORTB, PB0, ch1_startPWM, ch1_stopPWM, 0, 0 },
    { &OCR2B, 3, 4, 5, &PORTB, PB1, ch2_startPWM, ch2_stopPWM, 0, 0 },
};

static uint16_t readADC(uint8_t channel){
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // select channel, keep REFS bits
    (void)ADC;                                 // discard one read to let the MUX settle
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)){}
    return ADC;
}

static void setDuty(Channel *c, uint8_t newDuty){
    if (newDuty > TOP){ return; }
    uint8_t cur = *(c->duty_reg);
    if (cur > 0 && newDuty == 0)      { c->stopPWM();  }
    else if (cur == 0 && newDuty > 0) { c->startPWM(); }
    *(c->duty_reg) = newDuty;
}

// One regulation step for a single channel.
static void service_channel(Channel *c){
    // Pot is wired directly to its own ATmega328P ADC pin (not to the op-amp);
    // the MCU reads it as a plain analog input to get the setpoint (0..1023).
    uint16_t target = readADC(c->pot_adc);
    uint16_t v      = readADC(c->v_adc);
    uint8_t  duty   = *c->duty_reg;

    if (v < target && duty < TOP)      { setDuty(c, duty + 1); }
    else if (v > target && duty > 0)   { setDuty(c, duty - 1); }

    uint16_t i = readADC(c->i_adc);          // current limit
    if (i > MAX_SHUNT_ADC) { *c->flag_port |=  (1 << c->flag_pin); }
    else                   { *c->flag_port &= ~(1 << c->flag_pin); }

    c->last_v_adc = v;
    c->last_i_adc = i;
}

#ifdef WITH_DISPLAY
// Convert a channel's cached ADC readings into engineering units.
static long voutMillivolts(const Channel *c){
    return (long)c->last_v_adc * VOUT_MV_NUM / VOUT_MV_DEN;
}
static long currentMilliamps(const Channel *c){
    long ma = (IOPAMP_MV(c->last_i_adc) - IOFFSET_MV) * 1000L / ISLOPE_MV_PER_A;
    return ma < 0 ? 0 : ma;
}

// Render "n V=x.xx I=yyy P=zzz" for one channel into buf.
static void formatChannel(const Channel *c, uint8_t n, char *buf, uint8_t len){
    long mv = voutMillivolts(c);
    long ma = currentMilliamps(c);
    long mw = mv * ma / 1000L;                // P = V*I  (mV*mA/1000 = mW)
    snprintf(buf, len, "%u V%lu.%02lu I%lu P%lu",
             n,
             (unsigned long)(mv / 1000), (unsigned long)((mv % 1000) / 10),
             (unsigned long)ma, (unsigned long)mw);
}
#endif

int main(void){
    // ---- Control hardware ----
    DDRD |= (1 << DDD5) | (1 << DDD3);     // PWM outputs OC0B (PD5), OC2B (PD3)
    DDRB |= (1 << DDB0) | (1 << DDB1);     // over-current flags PB0, PB1

    // Channel 1 - Timer0, Fast PWM, TOP = OCR0A, no prescaler -> ~62.5 kHz
    TCCR0A = (1 << WGM01) | (1 << WGM00);  // non-inverting OC0B connected in setDuty()
    TCCR0B = (1 << WGM02) | (1 << CS00);
    OCR0A  = TOP; OCR0B = 0;

    // Channel 2 - Timer2, Fast PWM, TOP = OCR2A, no prescaler -> ~62.5 kHz
    TCCR2A = (1 << WGM21) | (1 << WGM20);  // non-inverting OC2B connected in setDuty()
    TCCR2B = (1 << WGM22) | (1 << CS20);
    OCR2A  = TOP; OCR2B = 0;

    // ADC: AVcc reference, max prescaler for accuracy, enabled
    ADCSRA |= (1 << ADEN) | 0b111;
    ADMUX   = (1 << REFS0);

#ifdef WITH_DISPLAY
    // ---- Display ----
    i2c_init();
    c_canvas canvas;
    canvas.clear();

    char l1[24];
    char l2[24];
    uint16_t tick = 0;
#endif

    while (1){
        service_channel(&ch[0]);
        service_channel(&ch[1]);

#ifdef WITH_DISPLAY
        if (++tick >= DISPLAY_EVERY){
            tick = 0;
            formatChannel(&ch[0], 1, l1, sizeof(l1));
            formatChannel(&ch[1], 2, l2, sizeof(l2));

            pos_t p1; p1.x = 0; p1.y = 0;    // top half  -> channel 1
            pos_t p2; p2.x = 0; p2.y = 16;   // lower half -> channel 2

            canvas.draw.clear();             // reset the draw-command list
            canvas.draw.text(p1, l1);
            canvas.draw.text(p2, l2);
            canvas.clear();                  // wipe the panel
            canvas.update();                 // rasterize + flush the text
        }
#endif
    }
}
