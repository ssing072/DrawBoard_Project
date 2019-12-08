
/*
 * ADC.h
 *
 * Created: 12/2/2019 11:05:13 PM
 *  Author: ssury
 */ 
#ifndef __ADC_H_
#define __ADC_H_

#include <avr/io.h>

void ADC_Init();
int get_ADC(char ch);

#endif

void ADC_Init(){
	ADCSRA |= (1<<ADEN) |(1<ADPS2)|(1<ADPS1) |(1<<ADPS0);
	ADMUX = 0x40;
}

int get_ADC(char ch){
	ADMUX = 0x40 | (ch & 0x07); //Set which ADC channel between 0x01, 0x02, 0x04
	ADCSRA|=(1<<ADSC); //ADC conversion
	while(!(ADCSRA & (1<<ADIF))); //Wait for conversion to complete
	//Clear ADIF by writing one to it
	ADCSRA|=(1<<ADIF);
	return (ADC);
}