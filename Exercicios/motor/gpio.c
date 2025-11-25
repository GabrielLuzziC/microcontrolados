// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas A, E, F, ADC0, UART0 e Timer0
// Template disponibilizado do Prof. Guilherme Peron
// (Corrigido por Gemini)

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTF (0x0020) //bit 5 (Corrigido: PortF é bit 5, não 6)
#define GPIO_PORTE (0x0010) //bit 4 (Corrigido: PortE é bit 4, não 5)
#define GPIO_PORTA (0x0001) //bit 0
#define TIMER0 (0x0001)
#define UART0 (0x0001)
#define ADC0_MODULE (0x0001) // Bit 0 -> ADC0

// -------------------------------------------------------------------------------
// Função Hardware_Init
// Inicializa os ports A, E, F, ADC0, UART0 e Timer0
// -------------------------------------------------------------------------------
void GPIO_Init(void)
{
	/* ------ GPIO ------ */
	
	// 1a. Ativar o clock para as portas A, E, F
	// CORREÇÃO: Usar |= para não apagar outros clocks
	SYSCTL_RCGCGPIO_R |= (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF);
	// 1b. Espera as portas ficarem prontas
	while((SYSCTL_PRGPIO_R & (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF) ) != (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF)){}
		
	// --- Configuração Porta E (PE0, PE1, PE4) ---
	// PE0: Motor (Digital Out)
	// PE1: Motor (Digital Out)
	// PE4: Potenciômetro (Analog In - AIN9)
	GPIO_PORTE_AHB_DIR_R |= 0x03;   // PE0, PE1 como saída
	GPIO_PORTE_AHB_AFSEL_R |= 0x10;  // Habilita função alternativa em PE4 (AIN9)
	GPIO_PORTE_AHB_DEN_R |= 0x03;    // Habilita digital para PE0, PE1
	GPIO_PORTE_AHB_AMSEL_R |= 0x10;  // Habilita analógico em PE4 (desabilita digital)

	// --- Configuração Porta F (PF2) ---
	// PF2: L293 Enable (Digital Out)
	GPIO_PORTF_AHB_DIR_R |= 0x04;   // PF2 como saída
	GPIO_PORTF_AHB_AFSEL_R &= ~(0x04); // Garante GPIO
	GPIO_PORTF_AHB_DEN_R |= 0x04;   // Habilita digital para PF2
	GPIO_PORTF_AHB_AMSEL_R &= ~(0x04); // Garante digital
	
	GPIO_PORTF_AHB_DATA_R |= 0x04;  // Deixa o L293 (Enable) ligado
	
	// --- Configuração Porta A (PA0, PA1) ---
	// PA0: UART0 RX
	// PA1: UART0 TX
	GPIO_PORTA_AHB_AFSEL_R |= 0x03;   // Habilita função alternativa em PA0, PA1
	GPIO_PORTA_AHB_DEN_R |= 0x03;     // Habilita digital para PA0, PA1
	GPIO_PORTA_AHB_AMSEL_R &= ~(0x03);  // Garante digital
	GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_PCTL_R & 0xFFFFFF00) | 0x00000011; // Configura PCTL para UART0

		
	/* ------ AD-DA (ADC0) ------ */
	// (Seu código do ADC estava correto)
	SYSCTL_RCGCADC_R |= ADC0_MODULE;
	while ((SYSCTL_PRADC_R & ADC0_MODULE) != ADC0_MODULE) {}
	
	ADC0_PC_R = 0x01; // 125 ksps
	ADC0_SSPRI_R = 0x0123; // Prioridade SS3
	ADC0_ACTSS_R &= ~(0x0008); // Desativa SS3
	ADC0_EMUX_R &= ~0xF000; // Gatilho por software
	ADC0_SSMUX3_R = 9; // Canal AIN9 (PE4)
	ADC0_SSCTL3_R = 0x0006; // IE0 + END0
	ADC0_IM_R &= ~(0x0008); // Desabilita interrupção
	ADC0_ACTSS_R |= 0x0008; // Ativa SS3
	
	/* ------ UART0 ------ */
	
	SYSCTL_RCGCUART_R |= UART0;
	while ((SYSCTL_PRUART_R & (UART0)) != UART0) {}
	
	UART0_CTL_R &= ~(0x0001); // Desativa UART

	// CORREÇÃO: Valores para 120MHz (não 80MHz)
	// BRD = 120.000.000 / (16 * 9600) = 781,25
	UART0_IBRD_R = 520;  // (Inteiro)
	UART0_FBRD_R = 53;   // (Fração = 0,25 * 64 = 16)
	
	// CORREÇÃO: Paridade ÍMPAR (0x79)
	// Roteiro pedia: 8 bits, 2 stop bits, paridade ÍMPAR, FIFO
	UART0_LCRH_R = 0x79; // 0b0111 1001
	
	UART0_CC_R = 0x00; // Clock do sistema
	UART0_CTL_R |= 0x0301; // Ativa RX, TX e UARTEN
	
	/* ------ TIMER0A ------ */
	
	SYSCTL_RCGCTIMER_R |= TIMER0;
	while((SYSCTL_PRTIMER_R & TIMER0) != TIMER0) {}
	
	TIMER0_CTL_R = 0x0;    // Desabilitar TimerA
	TIMER0_CFG_R = 0x00;   // Modo 32 bits
	TIMER0_TAMR_R = 0x02;  // Modo periódico
	
	// CORREÇÃO: Valor para 1ms @ 120MHz (não 80MHz)
	TIMER0_TAILR_R = 79999; // 120.000 ticks
	
	TIMER0_ICR_R = 0x01;   // Limpa flag
	TIMER0_IMR_R |= 0x01;  // Habilita interrupção de timeout
	
	// (Seu código de prioridade estava correto)
	NVIC_PRI4_R = (4 << 28); // Prioridade 4
	NVIC_EN0_R = 1 << 19;  // Habilitar IRQ 19 (Timer0A)
	
	TIMER0_CTL_R |= 0x01;  // Habilitar TimerA
}	

// (Sua função ReadChar estava correta)
uint8_t UART0_ReadChar(void) {
    while (UART0_FR_R & (0x10)); // Espera RXFE (FIFO vazia)
    return (uint8_t)(UART0_DR_R & 0xFF);
}

// (Sua função WriteChar estava correta)
void UART0_WriteChar(uint8_t data) {
    while (UART0_FR_R & (1 << 5)); // Espera TXFF (FIFO cheia)
    UART0_DR_R = data;
}

// (Sua função de leitura do ADC estava correta)
uint32_t ADC0_Read_SS3(void)
{
    uint32_t result;
    ADC0_PSSI_R = 0x0008; // Inicia conversão
    while ((ADC0_RIS_R & 0x08) == 0); // Espera
    result = (ADC0_SSFIFO3_R & 0xFFF); // Lê
    ADC0_ISC_R = 0x0008; // ACK
    return result;
}