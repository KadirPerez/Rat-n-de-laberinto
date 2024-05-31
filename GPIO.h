
/*
 * GPIO.h
 *
 * Created: 3/3/2024 6:34:01 PM
 */ 

/*
	Enumeraci�n para la configuraci�n de los pines de salida del ATMega328P.
	INPUT: Corresponde a configurar el pin como un pin de entrada.
	OUTPUT: Corresponde a configurar el pin como un pin de salida.
*/
enum CONFIGURACION {
	INPUT, OUTPUT
};

/*
	Enumeraci�n para representar el estado del pin.
	HIGH: 1 l�gico.
	LOW: 0 l�gico.
*/
enum ESTADOPIN {
	LOW = 0,
	HIGH = 1,
	ALLLOW = 0b00000000,
	ALLHIGH = 0b11111111
};


int verificarPin(char puerto, uint8_t numeroPin);
volatile uint8_t* registrarPuerto(char puerto);
volatile uint8_t* obtenerPuerto(char puerto);
volatile uint8_t* obtenerPin(char puerto);
void configurarPin(char puerto, uint8_t numeroPin, enum CONFIGURACION tipoPin);
void configurarPuerto(char puerto, enum CONFIGURACION tipoPuerto);
enum ESTADOPIN leerPin(char puerto, uint8_t numeroPin);
void escribirPin(char puerto, uint8_t numeroPin, enum ESTADOPIN estado);
uint8_t leerPuerto(char puerto);
void escribirPuerto(char puerto, uint8_t estado);