#include "lcd.h"
#include "tm4c1294ncpdt.h"

void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);

// Função que inicializa o LCD
void LCD_Init(void) {
	SysTick_Wait1ms(20);     // Espera inicial (estabilização do LCD)

	LCD_Command(0x38);       // Modo 2 linhas, matriz 5x7
	SysTick_Wait1us(40);

	LCD_Command(0x06);       // Cursor incrementa à direita
	SysTick_Wait1us(40);

	LCD_Command(0x0E);       // Display ON, cursor ON, não pisca
	SysTick_Wait1us(40);

	LCD_Command(0x01);       // Limpa o display
	SysTick_Wait1ms(2);
}

// Função que realiza comando no LCD
void LCD_Command(uint8_t cmd) {
	uint32_t temp_m;
	temp_m = GPIO_PORTM_DATA_R & 0xFC; // RS = 0 (comando) & RW = 0 (escrita)
	GPIO_PORTM_DATA_R = temp_m;
	GPIO_PORTK_DATA_R = cmd;    // Envia dados no barramento

	GPIO_PORTM_DATA_R |= 0x04;  // EN = 1
	SysTick_Wait1us(1);
	GPIO_PORTM_DATA_R &= 0xFB;  // EN = 0
	SysTick_Wait1us(40);
}

// Função que mostra um dado no LCD
void LCD_Data(uint8_t data) {
	uint32_t temp_m;
	
	GPIO_PORTM_DATA_R |= 0x01;  // RS = 1 (dado)
	temp_m = GPIO_PORTM_DATA_R & 0xFD; // RW = 0 (escrita)
	GPIO_PORTM_DATA_R = temp_m;
	
	GPIO_PORTK_DATA_R = data;   // Envia dados no barramento

	GPIO_PORTM_DATA_R |= 0x04;  // EN = 1
	SysTick_Wait1us(1);
	
	GPIO_PORTM_DATA_R &= 0xFB;  // EN = 0
	SysTick_Wait1us(40);
}

// Função que escreve a string recebida no LCD
void LCD_Write_String(char *str) {
	int i = 0;
	while(str[i] != '\0')
	{
		if(i == 16)
		{	
			LCD_Command(0xC0);
		}
		LCD_Data(str[i]);
		i++;
	}
}

// Função que limpar a tela do display
void LCD_Clean_Display(void) {
	LCD_Command(0x01);       // Limpa o display
	SysTick_Wait1ms(2);
}
