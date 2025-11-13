// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// LAB 2 - Cofre Eletrônico
// Template disponibilizado pelo Prof. Guilherme Peron
// Alunos: Gabriel Luzzi Correa & Thomas Pinton 

/* ----------------- Includes  ------------------ */
#include <stdint.h>
#include <string.h>
#include "lcd.h"
#include "teclado.h"
#include "motor.h"

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

void cofreAberto(char* senha);
void cofreFechando(void);
void cofreFechado(char* senha);
void cofreAbrindo(void);
void cofreTravado(void);

void acendeLED(void);
void apagaLED(void);

/* ------------ Estados do sistema --------------- */
typedef enum state{
    COFRE_ABERTO,
    COFRE_FECHANDO,
    COFRE_FECHADO,
    COFRE_ABRINDO,
    COFRE_TRAVADO
} state;

/* ------------- Variáveis Globais --------------- */
const char* senhaMestra = "1234";
state stateSistema = COFRE_ABERTO;
#define PASSO_COMPLETO 2048
#define MEIO_PASSO 4096
#define MODO_MEIO_PASSO 0
#define MODO_PASSO_COMPLETO 1
#define SENT_ANTIHORARIO 1
#define SENT_HORARIO 0

/* -------------- Loop Principal ----------------- */
int main(void)
 {
	// Inicializações de periféricos e configurações da placa
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	LCD_Init();	
	Motor_Init();
	
	char senha[5] = {0};

	// Loop infinito que verifica o estado do sistema
	while(1) {
		switch(stateSistema) {
			case COFRE_ABERTO:
				cofreAberto(&senha);
				break;
			case COFRE_FECHANDO:
				cofreFechando();
				break;
			case COFRE_FECHADO:
				cofreFechado(&senha);
				break;
			case COFRE_ABRINDO:
				cofreAbrindo();
				break;
			case COFRE_TRAVADO:
				cofreTravado();
				break;		
		}
	}
}

// Função que realiza a rotina do estado Cofre Aberto
void cofreAberto(char* senha)
{
	int senhaIndex = 0;
	for (int i = 0; i < 4; i++)
		senha[i] = '\0';
	
	LCD_Clean_Display();
	LCD_Write_String("Cofre aberto, digite nova senha");
	
	while(1) {
		char teclado = Varrer_Teclado();
		
		if (teclado != (char)0xFF && teclado != '#' && senhaIndex < 4)
		{
			LCD_Clean_Display();
			senha[senhaIndex] = teclado;
			(senhaIndex)++;
			LCD_Write_String(senha);
		} 
		else if (teclado == '#' && senhaIndex == 4)
		{
				stateSistema = COFRE_FECHANDO;
				return;
		}

		SysTick_Wait1ms(500);
	}
	return;
}

// Função que realiza a rotina do estado Cofre Fechando
void cofreFechando(void)
{
	LCD_Clean_Display();
	LCD_Write_String("Cofre fechando");
	
	SysTick_Wait1ms(1000);

	// Motor gira 2 voltas no sentido anti-horário (meio passo)
	Motor_Girar(2*MEIO_PASSO, SENT_ANTIHORARIO, MODO_MEIO_PASSO);

	stateSistema = COFRE_FECHADO;
	return;
}

// Função que realiza a rotina do estado Cofre Fechado
void cofreFechado(char* senha)
{
	LCD_Clean_Display();
	LCD_Write_String("Cofre fechado");
	stateSistema = COFRE_FECHADO;
	
	int tentativas = 0;
	char tentativa[5] = {0};
	int tentativaIndex = 0;
	
	while(1) {
		char teclado = Varrer_Teclado();
		
		if (teclado != (char)0xFF && teclado != '#' && tentativaIndex < 4)
		{
			LCD_Clean_Display();
			tentativa[tentativaIndex] = teclado;
			(tentativaIndex)++;
			LCD_Write_String(tentativa);
		} 
		else if (teclado == '#' && tentativaIndex == 4)
		{
				esperarSoltarTecla();
				LCD_Clean_Display();
				if (strcmp(senha, tentativa) == 0)
				{
					stateSistema = COFRE_ABRINDO;
					return;
				}
				else
				{
					for (int i = 0; i < 4; i++)
						tentativa[i] = '\0';
					tentativaIndex = 0;
					tentativas++;
					SysTick_Wait1ms(500);
					if (tentativas >= 3)
					{
						stateSistema = COFRE_TRAVADO;
						return;
					}
				}
		}
		SysTick_Wait1ms(500);
	}
	return;
}

// Função que realiza a rotina do estado Cofre Abrindo
void cofreAbrindo(void)
{
	LCD_Clean_Display();
	LCD_Write_String("Cofre abrindo");

	// Motor gira 2 voltas no sentido horário (passo completo)
	Motor_Girar(2*PASSO_COMPLETO, SENT_HORARIO, MODO_PASSO_COMPLETO);

	stateSistema = COFRE_ABERTO;
	return;
}

// Função que realiza a rotina do estado Cofre Travado
void cofreTravado(void)
{
	acendeLED();
	SysTick_Wait1ms(250);
	if (PortJ_Input() == 0x02)
	{
		char tentativa[5] = {0};
		int tentativaIndex = 0;
		
		while(1) {
			acendeLED();
			SysTick_Wait1ms(250);
			LCD_Clean_Display();
			char teclado = Varrer_Teclado();
			
			if (teclado != (char)0xFF && tentativaIndex < 4)
			{
				tentativa[tentativaIndex] = teclado;
				tentativaIndex++;
			} 
			else if (teclado == '#' && tentativaIndex == 4)
			{
				apagaLED();
				if (strcmp(senhaMestra, tentativa) == 0)
				{
					stateSistema = COFRE_ABRINDO;
					return;
				}
				else
				{
					break;
				}
			}
			LCD_Write_String(tentativa);
			apagaLED();
			SysTick_Wait1ms(250);
		}
	}
	
	apagaLED();
	LCD_Clean_Display();
	LCD_Write_String("Cofre travado");
	stateSistema = COFRE_TRAVADO;
	SysTick_Wait1ms(250);
	return;
}

// Função que acende LEDs da placa PAT
void acendeLED(void)
{
		PortQ_Output(0x0F);
    PortA_Output(0xF0);
		PortP_Output_Q1(0x50);
    //SysTick_Wait1ms(250);
}

//Função que apaga LEDs da placa PAT
void apagaLED(void)
{
		PortQ_Output(0x0);
    PortA_Output(0x0);
		PortP_Output_Q1(0x0);
    //SysTick_Wait1ms(250);
}
