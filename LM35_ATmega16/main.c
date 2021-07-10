/*
 * LM35_ATmega16.c
 *
 * Created: 5/19/2021 1:22:50 PM
 * Author : Linh
 */ 

// This program using AVRCodevision to run, all of the libraries are surport by Codevision.
// Auther: Nguyen Nhat Linh - Phung Van Si

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "LCD_16x2_H_file.h"
#include "LCD_16x2_C_file.c"

#define degree_sysmbol 0xdf

void ADC_Init(){
	DDRA = 0x00;							        /* Make ADC port as input */
	ADCSRA = 0x87;									/* Enable ADC, with freq/128  */
	ADMUX = 0x40;									/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	ADMUX = 0x40 | (channel & 0x07);				/* set input channel to read */
	ADCSRA |= (1<<ADSC);							/* Start ADC conversion */
	while (!(ADCSRA & (1<<ADIF)));					/* Wait until end of conversion by polling ADC interrupt flag */
	ADCSRA |= (1<<ADIF);							/* Clear interrupt flag */
	_delay_ms(1);									/* Wait a little bit */
	return ADCW;									/* Return ADC word */
}


int main(void)
{
	char Temperature[10];
	char ServoDegree[10];
	float celsius;
	float degree = 65;

	LCD_Init();                 /* initialize 16x2 LCD*/
	ADC_Init();					 /* initialize ADC*/

	DDRD |= (1<<PD5);
	DDRD |= (1<<PD0);               
	TCNT1 = 0;
	ICR1 = 2499;

	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	
	LCD_String_xy(1,0,"Nhiet do: ");
	LCD_String_xy(2,0,"Goc quay: ");

	while(1)
	{
		
		OCR1A = 65 + (ADC_Read(0)/4.35);
		degree = (ADC_Read(0)/4.35) / 1.3055556;
		//OCR1A = 175;
		//_delay_ms(1000);
		//OCR1A = 300;
		celsius = (ADC_Read(1)*4.88);
		celsius = (celsius/10.00);
		
		if((celsius > 55) && (PIND0 == 0))
		{
			PORTD = 0x01;
			OCR1A = 300;
			degree = 180;
			
			while(1)
			{
				celsius = (ADC_Read(1)*4.88);
				celsius = (celsius/10.00);
				sprintf(Temperature,"%d%cC  ", (int)celsius, degree_sysmbol);               /* convert integer value to ASCII string */
				LCD_String_xy(1,11,Temperature);
				sprintf(ServoDegree, "%d", (int)degree);
				LCD_String_xy(2,11,ServoDegree);                								/* send string data for printing */
				_delay_ms(500);
			
				if(celsius <= 55)
				{
					PORTD = 0x00;
					break;
				}
			}
			
		}
		
		
		sprintf(Temperature,"%d%cC  ", (int)celsius, degree_sysmbol);               /* convert integer value to ASCII string */
		LCD_String_xy(1,11,Temperature); 
		sprintf(ServoDegree, "%d", (int)degree); 
		LCD_String_xy(2,11,ServoDegree);                								/* send string data for printing */
		_delay_ms(500);
		memset(Temperature,0,10);
	



	}
	return 0;
}
