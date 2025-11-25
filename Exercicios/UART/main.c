// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// RX-TX UART
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

void PortN_Output(uint32_t valor);
void PortF_Output(uint32_t valor);
uint8_t UART0_ReadChar(void);
void UART0_WriteChar(uint8_t data);
void WriteString(char *str);

void acendeLED_N(uint32_t led);
void acendeLED_F(uint32_t led);
	
int main(void) {
	uint8_t led1_on = 0, led2_on = 0, led3_on = 0, led4_on = 0;
	PLL_Init();
	SysTick_Init();
	GPIO_Init();

	while(1) {
		char caractere = UART0_ReadChar();

		switch(caractere) {
			case '1':
				led1_on = !led1_on;  // alterna o estado
				if (led1_on) {
					acendeLED_N(0x2);  // PN1
					WriteString("LED 1 aceso\r\n");
				} else {
					acendeLED_N(0x0);  // apaga
					WriteString("LED 1 apagado\r\n");
				}
				break;

			case '2':
				led2_on = !led2_on;
				if (led2_on) {
					acendeLED_N(0x1);  // PN0
					WriteString("LED 2 aceso\r\n");
				} else {
					acendeLED_N(0x0);
					WriteString("LED 2 apagado\r\n");
				}
				break;

			case '3':
				led3_on = !led3_on;
				if (led3_on) {
					// PF4
					acendeLED_F(0x10);
					WriteString("LED 3 aceso\r\n");
				} else {
					acendeLED_F(0x0);
					WriteString("LED 3 apagado\r\n");
				}
				break;

			case '4':
				led4_on = !led4_on;
				if (led4_on) {
					// PF0
					acendeLED_F(0x01);
					WriteString("LED 4 aceso\r\n");
				} else {
					acendeLED_F(0x00);
					WriteString("LED 4 apagado\r\n");
				}
				break;

			default:
				break;
		}
	}

	return 0;
}

void acendeLED_N(uint32_t led) {
	PortN_Output(led);
}
void acendeLED_F(uint32_t led) {
	PortF_Output(led);
}

void WriteString(char *str) {
	while(*str != '\0') {
		UART0_WriteChar(*str++);
	}
}
	
