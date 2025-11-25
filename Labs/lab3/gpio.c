// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas A, N
// Template disponibilizado do Prof. Guilherme Peron

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTF  (0x0020) //bit 6
#define GPIO_PORTE	(0x0010) //bit 5
#define GPIO_PORTA  (0x0001) //bit 0
#define TIMER0 (0x0001)
#define UART0 (0x0001)
#define ADC0_MODULE (0x0001)   // Bit 0 -> ADC0

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports A, E, F
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF) ) != (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF)){}
		
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTE_AHB_AMSEL_R |= 0x10;
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
	
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTE_AHB_PCTL_R = 0x00;
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTF_AHB_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTE_AHB_DIR_R = 0x03; 	// bit0 e bit1
	GPIO_PORTF_AHB_DIR_R = 0x04;  // bit2
	
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTE_AHB_AFSEL_R |= 0x10; 
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
	
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTE_AHB_DEN_R |= 0x03;
	GPIO_PORTE_AHB_DEN_R &= ~(0x10);	//Desabilita digital do PE4
	GPIO_PORTA_AHB_DEN_R |= 0x03;
	GPIO_PORTF_AHB_DEN_R |= 0x04;
	
	// Deixa habilitado o Input do motor
	GPIO_PORTF_AHB_DATA_R = 0x04;
		
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	
	/* ------ AD-DA ------ */
	// 1. Habilita o clock do ADC0
	SYSCTL_RCGCADC_R |= ADC0_MODULE;
	while ((SYSCTL_PRADC_R & ADC0_MODULE) != ADC0_MODULE) {}
	
	// 2. Configuração básica do ADC0 (AD-DA)
	ADC0_PC_R = 0x01;         // 0x1 para 125 ksps
	ADC0_SSPRI_R = 0x0123;    // Prioridades dos sequenciadores (SS3 = mais alta)
	
	// 3. Desabilita SS3 antes da configuração (AD-DA)
	ADC0_ACTSS_R &= ~(0x0008);  // Desativa SS3
	
	// 4. Define gatilho por software (EM3 = 0000) (AD-DA)
	ADC0_EMUX_R &= ~0xF000;     // Bits 15–12 -> 0000	
	
	// 5. Seleciona o canal AIN9 (PE4) no SS3 (AD-DA)
	ADC0_SSMUX3_R = 9;          // Canal analógico 9

	// 6. Configura controle: IE0=1 (gera flag), END0=1 (última amostra) (AD-DA)
	ADC0_SSCTL3_R = 0x0006;       // 0110b -> IE0 + END0

	// 7. (Opcional) Interrupção do ADC (AD-DA)
	ADC0_IM_R &= ~(0x0008);     // Desabilita interrupção por enquanto

	// 8. Habilita novamente o SS3 (AD-DA)
	ADC0_ACTSS_R |= 0x0008;   // Ativa SS3
	
	/* ------ UART ------ */
	SYSCTL_RCGCUART_R |= UART0;
				
	while ((SYSCTL_PRUART_R & (UART0)) != UART0) {
    // Espera até a UART0 estar pronta
	}
	
	// Desativa o UARTEN
	UART0_CTL_R &= 0x0;

	// BRD = 80000000/(16* 9600) = 520,8333
	UART0_IBRD_R = 520;
	UART0_FBRD_R = 53;   // (0.8333 * 64) = 53
	
	// ATIVA 8 bits e habilita a Fila & 2 stopbit
	UART0_LCRH_R = 0x7A;   //	0111 1010
	
	// Usa o clock principal do sistema
	UART0_CC_R = 0x00;
	
	// Ativa o RX, TX e UARTEN
	UART0_CTL_R |= 0x0301;
	
	/* ------ TIMER ------ */
	// Configuração do TIMER0A - 32 bits
	SYSCTL_RCGCTIMER_R |= TIMER0;
	
	while((SYSCTL_PRTIMER_R & TIMER0) != TIMER0) {
		// Espera até a TIMER0 ficar pronto
	}
	
	// 2. Desabilitar TIMER0_CTL_R 
	TIMER0_CTL_R = 0x0;
	
	// 3. Colocar o TIMER0 no modo 32 bits (sem prescale)
	TIMER0_CFG_R = 0x00;
	
	// 4. Colocar no modo periódico
	TIMER0_TAMR_R = 0x02;
	
	TIMER0_TAPR_R = 0;
	
	// 5. Colocar o valor de contagem = 79.999
	TIMER0_TAILR_R = 79999;
	
	// 6. Limpa flag de interrupção no ICR
	TIMER0_ICR_R = 0x01;
	
	// 7a. Setar a interrupção do TimerA
	TIMER0_IMR_R |= 0x01; 
	
	// 7b. Setar prioridade de interrupção no NVIC
	NVIC_PRI4_R = (4 << 28);
	
	// 7c. Habilitar a interrupção do NVIC
	NVIC_EN0_R = 1 << 19;
	
	// 8. Habilitar a contagem no CTL
	TIMER0_CTL_R |= 0x01;
}

// -------------------------------------------------------------------------------
// Função UART0_ReadChar
// Lê um caractere do UART
// -------------------------------------------------------------------------------
uint8_t UART0_ReadChar(void) {
    // Espera enquanto a FIFO de recepção estiver vazia (RXFE = 1)
    while (UART0_FR_R & (0x10));
    
    // Quando RXFE = 0, há dados — lê e retorna
    return (uint8_t)(UART0_DR_R & 0xFF);
}

// -------------------------------------------------------------------------------
// Função UART0_WriteChar
// Escreve um caractere no UART
// -------------------------------------------------------------------------------
void UART0_WriteChar(uint8_t data) {
    // Espera enquanto a FIFO de transmissão estiver cheia (TXFF = 1)
    while (UART0_FR_R & (1 << 5));
    
    // Quando TXFF = 0, envia o dado
    UART0_DR_R = data;
}

// -------------------------------------------------------------------------------
// Função UART0_CharAvailable
// Verifica se tem algum dado disponível
// -------------------------------------------------------------------------------
int UART0_CharAvailable(void) {
    return !(UART0_FR_R & 0x10);   // returns 1 if data exists, 0 otherwise
}

// -------------------------------------------------------------------------------
// Função ADC0_Read_SS3
// Realiza uma conversão simples no canal AIN9 (PE4) e retorna o valor (0–4095)
// -------------------------------------------------------------------------------
uint32_t ADC0_Read_SS3(void)
{
    uint32_t result;

    // 1) Inicia a conversão (gatilho por software)
    ADC0_PSSI_R = 0x0008;

    // 2) Polling até o bit de conversão completa
    while ((ADC0_RIS_R & 0x08) == 0);

    // 3) Lê o resultado da FIFO
    result = (ADC0_SSFIFO3_R & 0xFFF);

    // 4) Limpa o flag (ACK)
    ADC0_ISC_R = 0x0008;

    // 5) Retorna o valor
    return result;
}
