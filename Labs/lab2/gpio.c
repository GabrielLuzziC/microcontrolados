// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas A, J, H, K, L, M, N, P e Q
// Template disponibilizado do Prof. Guilherme Peron

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTK  (0x0200) //bit 9
#define GPIO_PORTM	(0x0800) //bit 11
#define GPIO_PORTL	(0x0400) //bit 10
#define GPIO_PORTA  (0x0001) //bit 0
#define GPIO_PORTQ  (0x4000) //bit 14
#define GPIO_PORTP  (0x2000) //bit 13
#define GPIO_PORTH  (0x0080) //bit 7

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports A, J, H, K, L, M, N, P e Q
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTA | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTH );
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTA | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTH ) ) != 
			(GPIO_PORTJ | GPIO_PORTN | GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTA | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTH ) ){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
	GPIO_PORTA_AHB_PCTL_R = 0x00;	
	GPIO_PORTH_AHB_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTN_DIR_R = 0x03; 		 // BIT0 | BIT1
	
	/* --- Motor --- */
	GPIO_PORTH_AHB_DIR_R = 0x0F; // PH0-PH3
	
	/* --- LEDs PAT --- */
	GPIO_PORTA_AHB_DIR_R = 0xF0; // PA4-PA7
	GPIO_PORTQ_DIR_R = 0x0F;		 // PQ0-PQ3
	GPIO_PORTP_DIR_R = 0x20;		 // PP5
				
	/* --- LCD --- */
	GPIO_PORTK_DIR_R = 0xFF;		 // PK0-PK7
	GPIO_PORTM_DIR_R = 0x07; 		 // PM0, PM1, PM2
				
	/* --- Teclado Matricial --- */
	GPIO_PORTL_DIR_R = 0x00; 		 // PL0, PL1, PL2, PL3  (Entrada = 0)
	GPIO_PORTM_DIR_R |= 0x00; 	 // PM4, PM5, PM6, PM7 (Entrada Alta Impedância = 0)	
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00; 
	GPIO_PORTK_AFSEL_R = 0x00; 
	GPIO_PORTM_AFSEL_R = 0x00; 
	GPIO_PORTL_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00;
	GPIO_PORTP_AFSEL_R = 0x00;
	GPIO_PORTA_AHB_AFSEL_R = 0x00;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTJ_AHB_DEN_R = 0x03;  //Bit0 e bit1
	GPIO_PORTN_DEN_R = 0x03; 		  //Bit0 e bit1
	
	/* --- Motor --- */
	GPIO_PORTH_AHB_DEN_R = 0x0F;  // PH0-PH3
	
	/* --- LEDs PAT --- */
	GPIO_PORTA_AHB_DEN_R = 0xF0;  // PA4-PA7
	GPIO_PORTQ_DEN_R = 0x0F;		  // PQ0-PQ3
	GPIO_PORTP_DEN_R = 0x20;		  // PP5
	
	/* --- LCD --- */
	GPIO_PORTK_DEN_R = 0xFF;			// PK0-PK7 (LCD data)
	GPIO_PORTM_DEN_R = 0x07;			// PM0-PM2 (LCD control)
	
	/* --- Teclado Matricial --- */
	GPIO_PORTL_DEN_R = 0x0F;			// PL0-PL3
	GPIO_PORTM_DEN_R |= 0xF0;			// PM4-PM7
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;  //Bit0 e bit1	
	GPIO_PORTL_PUR_R = 0x0F;			// PL0-PL3
}	

// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}

// -------------------------------------------------------------------------------
// Função PortQ_Output
// Escreve valores no port Q
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: Não tem
void PortQ_Output(uint32_t valor)
{
	uint32_t temp;
	//vamos zerar somente os bits menos significativos
	//para uma escrita amigável nos bits 0 - 3
	temp = GPIO_PORTQ_DATA_R & 0xF0;
	//agora vamos fazer o OR com o valor recebido na função
	temp = temp | valor;
	GPIO_PORTQ_DATA_R = temp;
}

// -------------------------------------------------------------------------------
// Função PortA_Output
// Escreve valores no port A
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: Não tem
void PortA_Output(uint32_t valor)
{
	uint32_t temp;
	//vamos zerar somente os bits mais significativos
	//para uma escrita amigável nos bits 4 - 7
	temp = GPIO_PORTA_AHB_DATA_R & 0x0F;
	//agora vamos fazer o OR com o valor recebido na função
	temp = temp | valor;
	GPIO_PORTA_AHB_DATA_R = temp;
}

// -------------------------------------------------------------------------------
// Função PortP_Output_Q1
// Liga ou desliga transistor Q1 na PAT
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: Não tem
void PortP_Output_Q1(uint32_t valor) {
	uint32_t temp;
	//vamos zerar somente o bit 5
	//para uma escrita amigável no bit 5
	temp = GPIO_PORTP_DATA_R & 0xDF;
	//agora vamos fazer o OR com o valor recebido na função
	temp = temp | valor;
	GPIO_PORTP_DATA_R = temp;
}
