#ifndef TIMER0_H_
#define TIMER0_H_

void timer0inic() {
    OCR0A = 0x60; //Clear timer when it reaches this value
    TCCR0A |= (1 << WGM01); //Timer 0 Clear-Timer on Compare (CTC)
    TCCR0B |= (0b00000101); //Timer 0 prescaler = 1024
    TIMSK0 |= (1 << OCIE0A); //

    sei(); //Enable global interrupts
}

#endif /* TIMER0_H_ */

//Fosc=1/0.01Hz
//f_clk=8MHz
// N=1024
//F_osc=(f_ckl)/((1+ocr0a)*2.N)
