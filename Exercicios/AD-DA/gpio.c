// gpio_adc.c
// Desenvolvido para a placa EK-TM4C1294XL (TM4C1294NCPDT)
// Inicializa o Port E e o ADC0 (canal AIN9 - PE4)
// Baseado no template do Prof. Guilherme Peron
// Adaptado e simplificado para uso com ADC0 SS3

#include <stdint.h>
#include "tm4c1294ncpdt.h"

// Máscaras de clock
#define GPIO_PORTE  (0x0010)   // Bit 4 -> Porta E
#define ADC0_MODULE (0x0001)   // Bit 0 -> ADC0

// -------------------------------------------------------------------------------
// Função GPIO_ADC_Init
// Inicializa o Port E e configura o ADC0 para ler o canal AIN9 (PE4)
// -------------------------------------------------------------------------------
void GPIO_Init(void)
{
    // 1) Habilita o clock da porta E
    SYSCTL_RCGCGPIO_R |= GPIO_PORTE;
    while ((SYSCTL_PRGPIO_R & GPIO_PORTE) == 0);

    // 2) Habilita o clock do ADC0
    SYSCTL_RCGCADC_R |= ADC0_MODULE;
    while ((SYSCTL_PRADC_R & ADC0_MODULE) != ADC0_MODULE);

    // 3) Configura PE4 como entrada analógica
    GPIO_PORTE_AHB_DIR_R = 0x00;    // Entrada
    GPIO_PORTE_AHB_AFSEL_R |= 0x10;  // Sem função alternativa
		GPIO_PORTE_AHB_PCTL_R = 0x0000;
    GPIO_PORTE_AHB_DEN_R &= ~0x10;    // Desabilita digital
    GPIO_PORTE_AHB_AMSEL_R |= (0x10);   // Habilita modo analógico

    // 4) Configuração básica do ADC0
    ADC0_PC_R = 0x01;          // 0x3 = taxa de 250 ksps (pode usar 0x1 para 125 ksps)
    ADC0_SSPRI_R = 0x0123;    // Prioridades dos sequenciadores (SS3 = mais alta)

    // 5) Desabilita SS3 antes da configuração
    ADC0_ACTSS_R &= ~(0x0008);  // Desativa SS3

    // 6) Define gatilho por software (EM3 = 0000)
    ADC0_EMUX_R &= ~0xF000;     // Bits 15–12 -> 0000

    // 7) Seleciona o canal AIN9 (PE4) no SS3
    ADC0_SSMUX3_R = 9;          // Canal analógico 9

    // 8) Configura controle: IE0=1 (gera flag), END0=1 (última amostra)
    ADC0_SSCTL3_R = 0x0006;       // 0110b -> IE0 + END0

    // 9) (Opcional) Interrupção do ADC
    ADC0_IM_R &= ~(0x0008);     // Desabilita interrupção por enquanto

    // 10) Habilita novamente o SS3
    ADC0_ACTSS_R |= 0x0008;   // Ativa SS3
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
