// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Lab 3 - Controle bidirecional de motor DC
// Template disponibilizado pelo Prof. Guilherme Peron
// Alunos: Gabriel Luzzi Correa & Thomas Pinton 

/* ----------------- Includes  ------------------ */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "tm4c1294ncpdt.h"

#define PASSO 5

typedef enum state {
	PARADO,
	ESCOLHA,
	TERMINAL,
	POTENCIOMETRO
}state;

typedef enum sentidoRotacao {
	HORARIO,
	ANTI_HORARIO
} sentidoRotacao;
	
/* ------------- Variáveis Globais -------------- */
state estadoSistema = PARADO;
uint8_t pwmState = 0;
uint32_t duty = 0;
int velocidadeAtual = 0;

/* ------------------ Funções ------------------- */
void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);

uint8_t UART0_ReadChar(void);
int UART0_CharAvailable(void);
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

/* --------------- Loop Principal --------------- */
int main(void) {
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	
	while(1) {
		switch(estadoSistema) {
			case PARADO:
				stateParado();
				break;
			case ESCOLHA:
				stateEscolha();
				break;
			case TERMINAL:
				stateTerminal();
				break;
			case POTENCIOMETRO:
				statePotenciometro();
				break;
		}
	}

	return 0;
}

// Função que realiza rotina do estado parado
void stateParado()
{
	char caractere;
	for (int i = abs(velocidadeAtual); abs(velocidadeAtual) > 0; i-=PASSO)
	{
		girarMotor(0);
		SysTick_Wait1ms(50);
		mostrarVelocidadeTerminal();
	}
	
	WriteString("Motor parado, pressione '*' para iniciar\r\n");
	caractere = UART0_ReadChar();
	if(caractere == '*') {
		estadoSistema = ESCOLHA;
	}
}

// Função que realiza rotina do estado Escolha
void stateEscolha()
{
	char caractere;
	WriteString("Escolha o controle dos motores: potenciometro ('p') - terminal ('t'):\r\n");
	caractere = UART0_ReadChar();
	if(caractere == 'p') {
		estadoSistema = POTENCIOMETRO;
	} else if (caractere == 't') {
		estadoSistema = TERMINAL;
	}
}
	
// Função que realiza rotina do estado Terminal
void stateTerminal() {
	int velocidade = 0;
	char caractere = ' ';
	int stop = 0;
	sentidoRotacao sentido;
	
	WriteString("Escolha o sentido de rotacao: sentido horario ('h') ou anti-horario ('a')\r\n");
	
	while (caractere != 'h' && caractere != 'a') {
		caractere = UART0_ReadChar();
		
		if(caractere == 'h') {
			sentido = HORARIO;
		} else if (caractere == 'a') {
			sentido = ANTI_HORARIO;
		}
	}
	
	WriteString("Escolha a velocidade do motor: (5 - 0)\r\n");
	
	while ((caractere < '5' || caractere > '9') && caractere != '0') {	
		caractere = UART0_ReadChar();
	}
	
	velocidade = (caractere - '0') * 10;
	if (caractere == '0')
		velocidade = 99;
	
	if (sentido == ANTI_HORARIO)
		velocidade *= -1;
	
	girarMotor(velocidade);
	mostrarVelocidadeTerminal();
	
	while (1) {
		if (UART0_CharAvailable())
			checkCharTerminal(&velocidade, &stop);
		if (stop == 1) {
			estadoSistema = PARADO;
			return;
		}
		else {
			girarMotor(velocidade);
		}
		mostrarVelocidadeTerminal();
	}
}

// Função que checa e trata os caracteres recebidos no terminal
void checkCharTerminal(int* velocidade, int* stop)
{
	char caractere;
	
	while (1) {
		caractere = UART0_ReadChar();
	
		if(caractere == 'h') {
			if (velocidadeAtual < 0)
				*velocidade *= -1;
			return;
		} else if (caractere == 'a') {
			if (velocidadeAtual > 0)
				*velocidade *= -1;
			return;
		}
				
		if (caractere >= '5' && caractere <= '9' || caractere == '0') {	
			*velocidade = (caractere - '0') * 10;
			if (caractere == '0')
				*velocidade = 99;
			if (velocidadeAtual < 0)
				*velocidade *= -1;
			return;
		}
		
		if (caractere == 's') {
			*stop  = 1;
			return;
		}
	}
}

// Função que realiza rotina do estado Potenciômetro
void statePotenciometro() {
	mostrarVelocidadeTerminal();
	double velocidade = lerPotenciometro();
	if (velocidade >= 1.65) {
		girarMotor((int)(((velocidade-1.65)/1.65)*100));
	} else {
		girarMotor((int)(((1.65-velocidade)/1.65)*100*-1));
	}
	if (UART0_CharAvailable()) {
		char c = UART0_ReadChar();
		if (c == 's') {
			estadoSistema = PARADO;
		}
	}
}

// Função que mostra velocidade no terminal
void mostrarVelocidadeTerminal() 
{
	char str[10];
	WriteString("Velocidade motor: ");

	if (velocidadeAtual > 0) {
		snprintf(str, sizeof(str), "%d", velocidadeAtual);
		WriteString(str);
		WriteString(" | Sentido: horario\r\n");
	} else {
		snprintf(str, sizeof(str), "%d", velocidadeAtual);
		WriteString(str);
		WriteString(" | Sentido: anti-horario\r\n");
	}
}
	
// Função que seta uma velocidade alvo para o motor
// Dentro desta função é realizado a curva (linear) de aceleração/desaceleração
void girarMotor(int velocidadeAlvo) {
	// Velocidade: entre -99 e 99
	if (velocidadeAtual < velocidadeAlvo) {
		velocidadeAtual += PASSO;
		if (velocidadeAtual > velocidadeAlvo)
			velocidadeAtual = velocidadeAlvo;
	} else if (velocidadeAtual > velocidadeAlvo) {
		velocidadeAtual -= PASSO;
		if (velocidadeAtual < velocidadeAlvo)
			velocidadeAtual = velocidadeAlvo;
	}
	if (velocidadeAtual > 0)
		duty = (uint32_t) velocidadeAtual;
	else
		duty = (uint32_t)(velocidadeAtual * (-1));
}

// Função que retorna o sinal do potenciômetro
double lerPotenciometro() {
	uint32_t v;
	double volts;

	v = ADC0_Read_SS3();
	volts = (v * 3.3) / 4095.0;
	SysTick_Wait1ms(100);
		
	return volts;
}

// Função que printa a string na tela do terminal
void WriteString(char *str) {
	while(*str != '\0') {
		UART0_WriteChar(*str++);
	}
}

// Função de tratamento da Interrupção do Timer0A
void Timer0A_Handler(void)
{
	GPIO_PORTF_AHB_DATA_R |=(1 << 2);
	TIMER0_ICR_R = 1; // ACK
	GPIO_PORTE_AHB_DATA_R &= ~0x03; // zera PE0 e PE1 ANTES
	
	if(pwmState == 0) {
		if(velocidadeAtual > 0) { // horario
			GPIO_PORTE_AHB_DATA_R |= (1 << 0); // PE0 HIGH
		}
		else {
			GPIO_PORTE_AHB_DATA_R |= (1 << 1); // PE1 HIGH
		}

		pwmState = 1;
		TIMER0_TAILR_R = (80000 * duty) / 100;
	}
	else {
		if(velocidadeAtual < 0) { // anti-horario
			GPIO_PORTE_AHB_DATA_R &= ~(1 << 0); // PE0 LOW
		}
		else {
			GPIO_PORTE_AHB_DATA_R &= ~(1 << 1); // PE1 LOW
		}

		pwmState = 0;
		TIMER0_TAILR_R = (80000 * (100 - duty)) / 100;
	}
}
