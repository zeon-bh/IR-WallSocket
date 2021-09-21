/*
 * IR_WallSocket.c
 *
 * Created: 3/11/2018 8:11:32 PM
 * Author : Bharath
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>


#define RELAY1 PB1
#define RELAY2 PB2

#define IR PB0				// IR Modula Data Pin
#define MAXPULSE 65000

// IR Remote Button Definitions
#define POWER	0x3F67
#define BUTTON1 0x3B8E
#define BUTTON2 0x379C
#define BUTTON3 0x31FF
#define BUTTON4 0x3F8C

// Global Variables
uint16_t pulses[50][1];
uint8_t currentpulse = 0;
uint16_t ButtonData = 0;

// Function Prototypes
void ReadIR(void);
void GetButtonData(void);
void RelayControl(uint16_t);
void ClearPulses(void);

int main(void)
{
    // Set Relay Pins to HIGH Output
	DDRB |= (1<<RELAY1) | (1<<RELAY2);
	PORTB |= (1<<RELAY1) | (1<<RELAY2);
	
    while (1) 
    {		
		ReadIR();
		if(currentpulse >= 32){
			GetButtonData();
			RelayControl(ButtonData);
			ClearPulses();
			currentpulse = 0;	
		}	
    }
}

void ReadIR(void){
	uint16_t highpulse,lowpulse;
	highpulse = lowpulse = 0;
	
	// Read High Pulses
	while(PINB & (1<<IR)){
		highpulse++;
		// 20 us Resolution
		_delay_us(20);
		if((highpulse >= MAXPULSE) & (currentpulse != 0)){
			currentpulse = 0;
		}
	}
	// Store High Pulses
	pulses[currentpulse][0] = highpulse;
	
	// Read Low Pulses
	while(!(PINB & (1<<IR))){
		lowpulse++;
		// 20 us Resolution
		_delay_us(20);
		if((lowpulse >= MAXPULSE) & (currentpulse != 0)){
			currentpulse = 0;
		}
	}
	// Store Low pulses
	pulses[currentpulse][1] = lowpulse;
	
	//After Reading a pair of High-Low pulse
	currentpulse++;
}

void GetButtonData(void){
	uint8_t IRbit = 0;
	ButtonData = 0;
	for(uint8_t index = 18; index <= 33; index++){		// Sample the last 16 bits of the signal pulse
		if((pulses[index][0] + pulses[index][1]) > 90){
			ButtonData |= (1<<IRbit);
			IRbit++;
			} else {
			IRbit++;
		}
	}
}

void RelayControl(uint16_t irdata){
	if(ButtonData == POWER){
		PORTB ^= (1<<RELAY1);
		PORTB ^= (1<<RELAY2); 
	} else if(ButtonData == BUTTON1){
		PORTB ^= (1<<RELAY1);
	} else if(ButtonData == BUTTON2){
		PORTB ^= (1<<RELAY2);
	}
}

void ClearPulses(void){
	for(int i = 0; i <= 1; i++){
		for(int j = 0; j < currentpulse; j++){
			pulses[j][i] = 0;
		}
	}
}
