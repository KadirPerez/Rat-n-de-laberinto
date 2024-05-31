#define F_CPU 16000000L
#include <stdio.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "GPIO.h"
#include "UART.h"

int flag = 0; 
char adc_str[20];
bool vuelta = false;
int distanciaLateral = 0;
int distanciaInicialD = 0;
int distanciaInicialI = 0;
bool iniciado = false;

void generarTriger(int sensor){
	if(sensor == 1) {
		escribirPin('B', 1, HIGH);
		_delay_us(10);
		escribirPin('B', 1, LOW);
	} else {
		escribirPin('B', 2, HIGH);
		_delay_us(10);
		escribirPin('B', 2, LOW);
	}
}

ISR(TIMER1_CAPT_vect){
	_delay_ms(10);
	if(flag == 1) flag = 2;
}

void voltearLlanta(char llanta, char direccion){
	if(llanta == 'D') {
		if(direccion == 'D') {
			escribirPin('D', 4, HIGH);
			escribirPin('D', 7, LOW);
		} else {
			escribirPin('D', 4, LOW);
			escribirPin('D', 7, HIGH);
		}
	} else {
		if(direccion == 'D') {
			escribirPin('D', 2, HIGH);
			escribirPin('D', 3, LOW);
		} else {
			escribirPin('D', 2, LOW);
			escribirPin('D', 3, HIGH);
		}
	}
}

void darVuelta() {
	OCR0A = 0;
	OCR0B = 0;
	_delay_ms(100);
	voltearLlanta('I', 'A');
	
	OCR0A = 255;
	OCR0B = 255;
	_delay_ms(300);
	
	voltearLlanta('I', 'D');
	OCR0A = 200;
	OCR0B = 255;
}

void alejarseDeLaPared(){
	OCR0A = 0;
	OCR0B = 0;
	_delay_ms(100);
	voltearLlanta('D', 'A');
	voltearLlanta('I', 'A');
	OCR0A = 255;
	OCR0B = 255;
	_delay_ms(50);
	voltearLlanta('D', 'D');
	voltearLlanta('I', 'D');
}

void seguirPared(char pared){
	if(pared == 'D'){
		if(ICR1 < distanciaInicialD) {
			OCR0A = 150;
			OCR0B = 25;
			} else {
			OCR0A = 25;
			OCR0B = 150;
		}
	} else {
		if(ICR1 < distanciaInicialD) {
			OCR0B = 150;
			OCR0A = 25;
		} else {
			OCR0B = 50;
			OCR0A = 25;
		}
	}
}

int main(void) {
	
	//Medir voltaje
	ADMUX |= (0<<REFS1)|(0<<REFS0);
	ADMUX |= (0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
	ADCSRB |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	DIDR0 |= (1<<ADC0D);
	ADCSRA |= (1<<ADEN);
	
	//Sensores
	configurarPin('B', 0, INPUT);
	configurarPin('B', 1, OUTPUT);
	configurarPin('B', 2, OUTPUT);
	
	// Motores
	for (int i = 2; i <= 7; i++) configurarPin('D', i,OUTPUT);
	escribirPin('D', 2, HIGH);
	escribirPin('D', 4, HIGH);
	escribirPin('D', 5, HIGH);
	escribirPin('D', 6, HIGH);
	escribirPin('D', 3, LOW);
	escribirPin('D', 7, LOW);
	
	//timer como captura
	TCCR1B |= (1<<ICNC1) | (0<<ICES1) | (1<<CS12) | (0<CS11) | (1<<CS10);
	TIFR1 = 1<<ICF1;
	TIMSK1 |= (1<<ICIE1);
	sei();
	
	// timer pwm
	TCCR0A |= (1 << COM0A1) | (0 << COM0A0);
	TCCR0A |= (1 << COM0B1) | (0 << COM0B0);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (0 << WGM02);
	TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00);
	OCR0A = 100; //Derecha
	OCR0B = 150; //Izquierda
	
	UART_init(9600, 8, 0, 1, 0, 1, 0);
		
	while (1) {		
		
		//leer distancia infrarojo
		ADCSRA |= (1<<ADSC);
		while(!(ADCSRA & (1<<ADIF)));
		ADCSRA |= (1<<ADIF);
		
		generarTriger(1);
		
		while(1) {	
			if(leerPin('B', 0) == HIGH && flag == 0){
				TCNT1 = 0;
				flag = 1;
			} else if (flag == 2) {
				if (!iniciado) {
					distanciaInicialD = ICR1;
					iniciado = true;
				}
				break;
			}	
		}
		flag = 0;
		
		sprintf(adc_str, "%d", ADC);
		UART_string(adc_str);
		
		if(ICR1 < 20) seguirPared('D');
		
		if(ADC < 250) alejarseDeLaPared();
		
		if(ADC < 300) darVuelta();
	}
}