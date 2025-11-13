#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "motor.h"

#define MODO_PASSO_COMPLETO 1

void SysTick_Wait1ms(uint32_t delay);

// Função que inicializa os motores desligados
void Motor_Init(void) 
{
    GPIO_PORTH_AHB_DATA_R = 0x0F;  // inicializa os motores desligado
}

// Função aciona bobina e gira o motor em meio passo
void Motor_Meio_Passo(int passo)
{
	 switch(passo)
	{
		case 0: 
			GPIO_PORTH_AHB_DATA_R = 0x0E; 
			break;	// 1110
		case 1: 
			GPIO_PORTH_AHB_DATA_R = 0x0C;
			break;	// 1100
		case 2: 
			GPIO_PORTH_AHB_DATA_R = 0x0D;
			break;	// 1101
		case 3: 
			GPIO_PORTH_AHB_DATA_R = 0x09; 
			break;	// 1001
		case 4: 
			GPIO_PORTH_AHB_DATA_R = 0x0B; 
			break; 	// 1011
		case 5: 
			GPIO_PORTH_AHB_DATA_R = 0x03; 
			break; 	// 0011
		case 6:
			GPIO_PORTH_AHB_DATA_R = 0x07; 
			break; 	// 0111
		case 7: 
			GPIO_PORTH_AHB_DATA_R = 0x06; 
			break; 	// 0110
	}
}	

// Função aciona bobina e gira o motor em passo completo
void Motor_Passo_Completo(int passo)
{
	// Lógica invertida: 0 ativa, 1 desliga
	switch(passo)
	{
		case 0: 
			GPIO_PORTH_AHB_DATA_R = 0x0E;  
			break;	// 1110
		case 1: 
			GPIO_PORTH_AHB_DATA_R = 0x0D; 
			break;	// 1101
		case 2: 
			GPIO_PORTH_AHB_DATA_R = 0x0B; 
			break;	// 1011
		case 3: 
			GPIO_PORTH_AHB_DATA_R = 0x07;  
			break;	// 0111
	}
}

// Função que gira o motor em determinado número de passos e sentido
// modo -> 1 passo completo / 0 meio passo
void Motor_Girar(int passos, int sentido, int modo)
{
	// Passo completo = 4 ou Meio Passo = 8
	int passos_modo = (modo == MODO_PASSO_COMPLETO) ? 4 : 8;
	
	for (int i = 0; i < passos; i++)
	{
			int p = sentido ? (i % passos_modo) : (passos_modo - 1 - (i % passos_modo));

			if (modo == MODO_PASSO_COMPLETO)
					Motor_Passo_Completo(p);
			else
					Motor_Meio_Passo(p);

			SysTick_Wait1ms(5);
	}
	GPIO_PORTH_AHB_DATA_R = 0x0F; // desliga todas as bobinas
}
