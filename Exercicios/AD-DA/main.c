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

uint32_t ADC0_Read_SS3(void);
	
int main(void) {
	uint32_t v;
	double volts;
	
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	

	
	while(1) {
		v = ADC0_Read_SS3();
		volts = (v * 3.3) / 4095.0;
		SysTick_Wait1ms(500);
	}
}

	
