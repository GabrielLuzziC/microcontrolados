// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Lab 3 - Controle bidirecional de motor DC
// Template disponibilizado pelo Prof. Guilherme Peron
// Alunos: Gabriel Luzzi Correa & Thomas Pinton 

/* ----------------- Includes  ------------------ */
#include <stdint.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"

#define PASSO 5

typedef enum sentidoRotacao {
	HORARIO,
	ANTI_HORARIO
} sentidoRotacao;
	
/* ------------- Variáveis Globais -------------- */
uint8_t pwmState = 0;
uint32_t duty = 80;
sentidoRotacao sentido = ANTI_HORARIO;

/* ------------------ Funções ------------------- */
void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);

uint8_t UART0_ReadChar(void);
void UART0_WriteChar(uint8_t data);
void WriteString(char *str);

void stateParado(void);
void stateEscolha(void);
void stateTerminal(void);
void statePotenciometro(void);

void checkCharTerminal(int* velocidade, int* stop);
void girarMotor(int velocidade);
void mostrarVelocidadeTerminal(void);
double lerPotenciometro(void);
void Timer0A_Handler(void);
uint32_t ADC0_Read_SS3(void);


int main(void) {
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	
	
	while(1) {
		girarMotor(50);
	}
	/*
	while(1) {
		GPIO_PORTE_AHB_DATA_R = 0x01;
	}*/
	
	return 0;
}

void girarMotor(int velocidade) {
	// Velocidade: entre 00 e 99
	uint32_t velocidadeAlvo = (uint32_t)velocidade;
	if (duty < velocidadeAlvo) {
		duty += PASSO;
		if (duty > velocidadeAlvo)
			duty = velocidadeAlvo;
	} else if (duty > velocidadeAlvo) {
		duty -= PASSO;
		if (duty < velocidadeAlvo)
			duty = velocidadeAlvo;
	}
}

/* ------- Interrupção Timer0A ------ */
void Timer0A_Handler(void)
{
	GPIO_PORTF_AHB_DATA_R |=(1 << 2);
	TIMER0_ICR_R = 1; // ACK

	if(pwmState == 0) {
		if(sentido == HORARIO) {
			GPIO_PORTE_AHB_DATA_R |= (1 << 0); // PE0 HIGH
		}
		else {
			GPIO_PORTE_AHB_DATA_R |= (1 << 1); // PE1 HIGH
		}

		pwmState = 1;
		TIMER0_TAILR_R = (80000 * duty) / 100;
	}
	else {
		if(sentido == HORARIO) {
			GPIO_PORTE_AHB_DATA_R &= ~(1 << 0); // PE0 LOW
		}
		else {
			GPIO_PORTE_AHB_DATA_R &= ~(1 << 1); // PE1 LOW
		}

		pwmState = 0;
		TIMER0_TAILR_R = (80000 * (100 - duty)) / 100;
	}
}


