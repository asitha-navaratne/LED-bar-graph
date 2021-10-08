#define F_CPU 8000000UL

#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#define SHIFT_DDR	DDRB			///< DDR of 74HC595 shift register.
#define SHIFT_PORT	PORTB			///< Port of shift register.
#define SHIFT_PIN	PB0				///< Pin for shift register clock.
#define LATCH_PIN	PB2				///< Pin for storage register clock.
#define DATA_PIN	PB1				///< Pin for sending serial data to shift register.

void PORT_INIT(void);
void ADC_INIT(void);
uint16_t ADC_CONVERSION(void);
void SERIAL_PULSE(void);
void LATCH_PULSE(void);

int main(void){
	PORT_INIT();
	ADC_INIT();
	
	uint16_t result = 0;
	uint8_t power = 0;
	uint8_t display = 0;
	
	while(1){
		result = ADC_CONVERSION();
		power = result/127;
		display = pow(2,power) - 1;
		if(display){
			display = display | 0x01;
		}
		for(uint8_t i=0;i<8;i++){
			if(display & (1<<i)){
				SHIFT_PORT |= (1<<DATA_PIN);
			}
			else{
				SHIFT_PORT &= ~(1<<DATA_PIN);
			}
			SERIAL_PULSE();							///< Send the number to the shift register serially.
		}
		LATCH_PULSE();								///< Output data in the shift register (parallelly).
	}
}

/*!
 *	@brief Initialize Ports.
 */

void PORT_INIT(void){
	SHIFT_DDR |= (1<<SHIFT_PIN)|(1<<LATCH_PIN)|(1<<DATA_PIN);
}

/*!
 *	@brief Initialize ADC.
 */

void ADC_INIT(void){
	ADMUX = (1<<REFS0);								///< Set ADC reference to AVCC and analog input mode to single ended input of ADC0.
	ADCSRA = ((1<<ADEN)|(1<<ADPS2)|(1<<ADPS1));		///< Enable ADC and set ADC prescalar to 64.
}

/*!
 *	@brief Perform an ADC conversion.
 *	@return Result of ADC conversion (uint16_t).
 */

uint16_t ADC_CONVERSION(void){
	ADCSRA |= (1<<ADSC);								///< Start ADC conversion.
	while(ADCSRA & (1<<ADSC));							///< Wait until conversion is complete.
	return ADC;											///< Return result of conversion.
}

/*!
 *	@brief Send a clock pulse to serial register clock pin to transmit one bit to the shift register.
 */

void SERIAL_PULSE(void){
	SHIFT_PORT |= (1<<SHIFT_PIN);
	_delay_us(10);
	SHIFT_PORT &= ~(1<<SHIFT_PIN);
	_delay_us(2);
}

/*!
 *	@brief Send a clock pulse to storage register clock pin (Latch pin) to output the data in shift register.
 */

void LATCH_PULSE(void){
	SHIFT_PORT |= (1<<LATCH_PIN);
	_delay_us(50);
	SHIFT_PORT &= ~(1<<LATCH_PIN);
}
