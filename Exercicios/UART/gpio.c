// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas A, N
// Template disponibilizado do Prof. Guilherme Peron

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTF  (0x0020) //bit 6
#define GPIO_PORTA  (0x0001) //bit 0
#define UART0 (0x0001)

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports A, N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTN | GPIO_PORTA | GPIO_PORTF);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTN | GPIO_PORTA | GPIO_PORTF) ) != (GPIO_PORTN | GPIO_PORTA | GPIO_PORTF)){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTF_AHB_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTN_DIR_R = 0x03; 		 // BIT0 | BIT1
	GPIO_PORTF_AHB_DIR_R = 0x11;  // bit0 e bit4
					
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTN_AFSEL_R = 0x00; 
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTN_DEN_R = 0x03;
	GPIO_PORTA_AHB_DEN_R = 0x03;
	GPIO_PORTF_AHB_DEN_R = 0x11;
		
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	
	/* ------ UART ------ */
	
	SYSCTL_RCGCUART_R |= UART0;
				
	while ((SYSCTL_PRUART_R & (UART0)) != UART0) {
    // Espera até a UART0 estar pronta
	}
	
	// Desativa o UARTEN
	UART0_CTL_R &= 0x0;

	// BRD = 80000000/(16* 57600) = 86,8055
	UART0_IBRD_R = 86;
	UART0_FBRD_R = 51;   // (0.8055 * 64) = 51
	
	// ATIVA 8 bits e habilita a Fila & 1 stopbit
	UART0_LCRH_R = 0x70;   //	0111 0000
	
	// Usa o clock principal do sistema
	UART0_CC_R = 0x00;
	
	// Ativa o RX, TX e UARTEN
	UART0_CTL_R |= 0x0301;
}	

void PortN_Output(uint32_t valor)
{
	uint32_t temp;
	// Zera apenas os bits 0 e 1 (LEDs)
	temp = GPIO_PORTN_DATA_R & ~0x03U;  // limpa bits 0 e 1 (0b11)

	// Faz OR com o valor desejado (mantém os outros bits)
	temp |= (valor & 0x03);  // garante que só usa bits 0 e 1

	// Escreve no registrador
	GPIO_PORTN_DATA_R = temp;
}

void PortF_Output(uint32_t valor)
{
	uint32_t temp;
	// Zera apenas os bits 0 e 1 (LEDs)
	temp = GPIO_PORTF_AHB_DATA_R & ~0x11U;  // limpa bits 4 e 0 (0b11)

	// Faz OR com o valor desejado (mantém os outros bits)
	temp |= (valor & 0x11);  // garante que só usa bits 0 e 1

	// Escreve no registrador
	GPIO_PORTF_AHB_DATA_R = temp;
}

uint8_t UART0_ReadChar(void) {
    // Espera enquanto a FIFO de recepção estiver vazia (RXFE = 1)
    while (UART0_FR_R & (0x10));
    
    // Quando RXFE = 0, há dados — lê e retorna
    return (uint8_t)(UART0_DR_R & 0xFF);
}

void UART0_WriteChar(uint8_t data) {
    // Espera enquanto a FIFO de transmissão estiver cheia (TXFF = 1)
    while (UART0_FR_R & (1 << 5));
    
    // Quando TXFF = 0, envia o dado
    UART0_DR_R = data;
}

