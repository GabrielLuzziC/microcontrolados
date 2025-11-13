#include "teclado.h"
#include "tm4c1294ncpdt.h"

void SysTick_Wait1us(uint32_t delay);
void SysTick_Wait1ms(uint32_t delay);

// Função que realiza varredura do teclado
// Retorna um caractere
char Varrer_Teclado(void) {
	// Mapa de teclas do teclado matricial
	char mapa_teclas[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
	};
	
	const uint8_t linhas[] = {0x01, 0x02, 0x04, 0x08};  // PL0–PL3
	const uint8_t colunas[] = {0x10, 0x20, 0x40, 0x80}; // PM4–PM7
	int i, j;

	// Todas as colunas como entrada (alta impedância)
	GPIO_PORTM_DIR_R &= ~0xF0U; // PM4–PM7 = entrada

	for (i = 0; i < 4; i++)
	{
		// Ativa a coluna i (coloca PMi em 0)
		GPIO_PORTM_DIR_R |= colunas[i];    // Configura a coluna i como saída
		GPIO_PORTM_DATA_R &= ~colunas[i];  // Força nível 0

		SysTick_Wait1us(50);

		// Lê as linhas (PL0–PL3)
		uint8_t leitura = GPIO_PORTL_DATA_R & 0x0F;

		// Verifica se alguma linha está em 0 (tecla pressionada)
		for (j = 0; j < 4; j++)
		{
			if ((leitura & linhas[j]) == 0)
			{
				// Retorna a tecla correspondente (linha x coluna)
				GPIO_PORTM_DIR_R &= ~colunas[i]; // volta coluna para entrada
				return (mapa_teclas[j][i]); 		 
			}
		}

		// Desativa a coluna (volta para entrada)
		GPIO_PORTM_DIR_R &= ~colunas[i];
	}

	return 0xFF; // Nenhuma tecla pressionada
}

//Função que espera liberar todas as teclas
void esperarSoltarTecla(void)
{
	while (Varrer_Teclado() != 0xFF)
	{
		SysTick_Wait1ms(20); // espera até todas soltarem
	}
}

