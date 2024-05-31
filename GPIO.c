/*
* GPIO-Driver.c
*
* Created: 3/1/2024 2:48:27 PM
*/

#include <avr/io.h>
#include "GPIO.h"

/*
	Función para verificar un pin.
*/
int verificarPin(char puerto, uint8_t numeroPin) {
	// Verificamos que es un puerto válido.
	if (puerto == 'B' || puerto == 'C' || puerto == 'D') {
		// Verificamos que es un pin dentro del rango dependiendo del puerto. EL puerto 'B' y 'D' tienen 8 pines, mientras que el puerto 'C' tiene 7.
		if (((puerto == 'B' || puerto == 'D') && numeroPin >= 0 && numeroPin <= 7) || (puerto == 'C' && numeroPin >= 0 && numeroPin <= 6)) {
			return 1;
		}
	}
	
	// No se cumplen las condiciones, no es un puerto válido.
	return 0;
}

/*
	Función para obtener el registro del puerto.
*/
volatile uint8_t* registrarPuerto(char puerto) {
	switch (puerto) {
	case 'B':
		return &DDRB;
		break;
		
	case 'C':
		return &DDRC;
		break;
		
	case 'D':
		return &DDRD;
		break;
		
	default:
		return 0;
	}
}

volatile uint8_t* obtenerPuerto(char puerto) {
	switch (puerto) {
		case 'B':
			return &PORTB;
			break;
		
		case 'C':
			return &PORTC;
			break;
			
		case 'D':
			return &PORTD;
			break;
			
		default:
			return 0;
	}
}


/*
	Función para obtener el pin del puerto.
*/
volatile uint8_t* obtenerPin(char puerto) {
	switch (puerto) {
		case 'B':
			return &PINB;
			break;
		
		case 'C':
			return &PINC;
			break;
		
		case 'D':
			return &PIND;
			break;
		
		default:
			return 0;
	}
}

/*
	Configurar un pin del ATMEGA328P como entrada o como salida.
	puerto: Puerto donde está el pin a configurar. B/C/D.
	numeroPin: Número de pin a configurar. Rango válido de 0 a 7.
	tipoPin: Configuración del pin. INPUT/OUTPUT.
*/
void configurarPin(char puerto, uint8_t numeroPin, enum CONFIGURACION tipoPin) {
	// Verificamos que es un pin válido.
	if (verificarPin(puerto, numeroPin)) {
		// Determinar dirección del puerto.
		volatile uint8_t *puertoRegistro = registrarPuerto(puerto);
		
		// ref: https://eleckia.wordpress.com/2019/06/13/perifericos-atmega328p-puertos-de-entrada-salida-de-proposito-general-gpio/
		if (tipoPin == INPUT) {
			*puertoRegistro &= ~(1 << numeroPin); // Modificamos directamente el registro del puerto para marcar el pin como un pin de entrada.
		} else {
			*puertoRegistro |= (1 << numeroPin); // Modificamos directamente el registro del pueto para marca el pin como un pin de salida.	
		}
	}
}


/*
Configurar un puerto del ATMEGA328P como puerto de entrada o puerto de salida.
puerto: Nombre del puerto a configurar. 'B', 'C', 'D'.
tipoPuerto: Configuración del puerto. INPUT/OUTPUT.
*/
void configurarPuerto(char puerto, enum CONFIGURACION tipoPuerto) {
	// Verificar parámetro del puerto.
	if (puerto == 'B' || puerto == 'C' || puerto == 'D') {
		// Determinar dirección del puerto.
		volatile uint8_t *puertoRegistro = registrarPuerto(puerto); 
			
		// Modificar el registro del puerto para configurarlo como entrada o salida dependiendo de tipoPuerto.
		*puertoRegistro = tipoPuerto == INPUT ? 0x00 : 0xFF;
	}
}

/*
	Leer el estado de un pin.
	puerto: Puerto donde está el pin a configurar. B/C/D.
	numeroPin: Número de pin a configurar. Rango válido de 0 a 7.

	La función regresa un ESTADOPIN, ya sea HIGH o LOW.
*/
enum ESTADOPIN leerPin(char puerto, uint8_t numeroPin) {
	// Verificamos que es un pin válido.
	if (verificarPin(puerto, numeroPin)) {
		// Determinar registro del pin.
		volatile uint8_t *puertoRegistro = obtenerPin(puerto);
		
		// Con 0x01, desplazamos el bit del número de pin que se necesita hacia el bit menos significativo, de esta manera, este solo se tomará en cuenta para valorar su estado.
		return ((*puertoRegistro >> numeroPin) & 0x01) ? HIGH : LOW;
	}
	
	// No se cumplen las condiciones, se regresa un LOW.
	return LOW;
}

/*
	Escribir el estado del pin.
	puerto: Puerto donde está el pin a configurar. B/C/D.
	numeroPin: Número de pin a configurar. Rango válido de 0 a 7.
	estado: Valor que se quiere escribir en el pin. HIGH/LOW.
*/
void escribirPin(char puerto, uint8_t numeroPin, enum ESTADOPIN estado) {
	// Verificamos que es un pin válido.
	if (verificarPin(puerto, numeroPin)) {
		//Determinar el registro del puerto.
		volatile uint8_t *puertoRegistro = obtenerPuerto(puerto);
		
		// Escribir estado del pin.
		if (estado == LOW) {
			*puertoRegistro &= ~(1 << numeroPin);
		} else {
			*puertoRegistro |= (1 << numeroPin);
		}
	}	
}

/*
	Leer el estado del puerto utilizando el valor del primer pin que representa el valor de los demás.
	puerto: Puerto del que se quiere leer el estado.
*/
uint8_t leerPuerto(char puerto) {
	// Determinar el registro del pin.
	volatile uint8_t *puertoRegistro = obtenerPuerto(puerto);
	 // Leer el estado del puerto y regresar el valor de 8 bits.
	return *puertoRegistro;
}

/*
	Escribe el estado de todos los pines de un puerto.
	puerto: Pueto del que se quiere escribir el estado.
	estado: Estado que se quiere escribir.
*/
void escribirPuerto(char puerto, uint8_t estado) {
	// Determinar registro del puerto.
	volatile uint8_t *registroPuerto = obtenerPuerto(puerto);
	
	// Escribir el estado del puerto.
	*registroPuerto = estado;
}
