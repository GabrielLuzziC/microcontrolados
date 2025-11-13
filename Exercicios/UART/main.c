// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// LAB 2 - Cofre Eletrônico
// Template disponibilizado pelo Prof. Guilherme Peron
// Alunos: Gabriel Luzzi Correa & Thomas Pinton 

/* ----------------- Includes  ------------------ */
#include <stdint.h>

/* ------------------ Funções ------------------- */
void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);

uint32_t PortJ_Input(void);
void PortP_Output_Q1(uint32_t valor);
void PortQ_Output(uint32_t valor);
void PortA_Output(uint32_t valor);
void PortN_Output(uint32_t valor);
uint8_t UART0_ReadChar(void);

void acendeLED(uint32_t led);
void apagaLED(void);
	
int main(void) {
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	int i = 0;
	
	while(1) {
		char caractere = UART0_ReadChar();
		
		if (caractere){
			i++;
		}
			
		
		if (caractere == '0') {
			acendeLED(0x1);
		} 
		else if(caractere == '2') {
			acendeLED(0x2);
		}
		else if(caractere == '3') {
			//acendeLED(0x);
		}
		else if(caractere == '4') {
			//acendeLED(0x1);
		}
		
		acendeLED(0x1);
		SysTick_Wait1ms(500);
		acendeLED(0x0);
	}
	
	return 0;
}

void acendeLED(uint32_t led) {
	PortN_Output(led);
}
	
	
