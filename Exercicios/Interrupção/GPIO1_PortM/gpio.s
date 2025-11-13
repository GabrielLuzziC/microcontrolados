; gpio.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 19/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
; ========================
; Definições dos Registradores Gerais
SYSCTL_RCGCGPIO_R	 EQU	0x400FE608
SYSCTL_PRGPIO_R		 EQU    0x400FEA08
    
; ========================
; NVIC
NVIC_EN1_R           EQU    0xE000E104
NVIC_PRI12_R		 EQU	0xE000E430
; ========================
; Definições dos Ports
; PORT J
GPIO_PORTJ_AHB_DATA_BITS_R  EQU 0x40060000
GPIO_PORTJ_AHB_DATA_R       EQU 0x400603FC
GPIO_PORTJ_AHB_DIR_R        EQU 0x40060400
GPIO_PORTJ_AHB_IS_R         EQU 0x40060404
GPIO_PORTJ_AHB_IBE_R        EQU 0x40060408
GPIO_PORTJ_AHB_IEV_R        EQU 0x4006040C
GPIO_PORTJ_AHB_IM_R         EQU 0x40060410
GPIO_PORTJ_AHB_RIS_R        EQU 0x40060414
GPIO_PORTJ_AHB_MIS_R        EQU 0x40060418
GPIO_PORTJ_AHB_ICR_R        EQU 0x4006041C
GPIO_PORTJ_AHB_AFSEL_R      EQU 0x40060420
GPIO_PORTJ_AHB_DR2R_R       EQU 0x40060500
GPIO_PORTJ_AHB_DR4R_R       EQU 0x40060504
GPIO_PORTJ_AHB_DR8R_R       EQU 0x40060508
GPIO_PORTJ_AHB_ODR_R        EQU 0x4006050C
GPIO_PORTJ_AHB_PUR_R        EQU 0x40060510
GPIO_PORTJ_AHB_PDR_R        EQU 0x40060514
GPIO_PORTJ_AHB_SLR_R        EQU 0x40060518
GPIO_PORTJ_AHB_DEN_R        EQU 0x4006051C
GPIO_PORTJ_AHB_LOCK_R       EQU 0x40060520
GPIO_PORTJ_AHB_CR_R         EQU 0x40060524
GPIO_PORTJ_AHB_AMSEL_R      EQU 0x40060528
GPIO_PORTJ_AHB_PCTL_R       EQU 0x4006052C
GPIO_PORTJ_AHB_ADCCTL_R     EQU 0x40060530
GPIO_PORTJ_AHB_DMACTL_R     EQU 0x40060534
GPIO_PORTJ_AHB_SI_R         EQU 0x40060538
GPIO_PORTJ_AHB_DR12R_R      EQU 0x4006053C
GPIO_PORTJ_AHB_WAKEPEN_R    EQU 0x40060540
GPIO_PORTJ_AHB_WAKELVL_R    EQU 0x40060544
GPIO_PORTJ_AHB_WAKESTAT_R   EQU 0x40060548
GPIO_PORTJ_AHB_PP_R         EQU 0x40060FC0
GPIO_PORTJ_AHB_PC_R         EQU 0x40060FC4
GPIO_PORTJ               	EQU 2_00000100000000	
; PORT M
GPIO_PORTM_IS_R      	EQU    0x40063404
GPIO_PORTM_IBE_R      	EQU    0x40063408
GPIO_PORTM_IEV_R      	EQU    0x4006340C
GPIO_PORTM_IM_R      	EQU    0x40063410
GPIO_PORTM_RIS_R      	EQU    0x40063414
GPIO_PORTM_ICR_R      	EQU    0x4006341C    
GPIO_PORTM_LOCK_R    	EQU    0x40063520
GPIO_PORTM_CR_R      	EQU    0x40063524
GPIO_PORTM_AMSEL_R   	EQU    0x40063528
GPIO_PORTM_PCTL_R    	EQU    0x4006352C
GPIO_PORTM_DIR_R     	EQU    0x40063400
GPIO_PORTM_AFSEL_R   	EQU    0x40063420
GPIO_PORTM_DEN_R     	EQU    0x4006351C
GPIO_PORTM_PUR_R     	EQU    0x40063510	
GPIO_PORTM_DATA_R    	EQU    0x400633FC
GPIO_PORTM              EQU    2_00100000000000
; PORT N
GPIO_PORTN_LOCK_R    	EQU    0x40064520
GPIO_PORTN_CR_R      	EQU    0x40064524
GPIO_PORTN_AMSEL_R   	EQU    0x40064528
GPIO_PORTN_PCTL_R    	EQU    0x4006452C
GPIO_PORTN_DIR_R     	EQU    0x40064400
GPIO_PORTN_AFSEL_R   	EQU    0x40064420
GPIO_PORTN_DEN_R     	EQU    0x4006451C
GPIO_PORTN_PUR_R     	EQU    0x40064510	
GPIO_PORTN_DATA_R    	EQU    0x400643FC
GPIO_PORTN              EQU    2_001000000000000	


; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT GPIO_Init            ; Permite chamar GPIO_Init de outro arquivo
		EXPORT PortN_Output			; Permite chamar PortN_Output de outro arquivo
        EXPORT GPIOPortJ_Handler    
        IMPORT EnableInterrupts
        IMPORT DisableInterrupts
		IMPORT SysTick_Wait1ms
									

;--------------------------------------------------------------------------------
; Função GPIO_Init
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
GPIO_Init
;=====================
; 1. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO,
; após isso verificar no PRGPIO se a porta está pronta para uso.
; enable clock to GPIOF at clock gating register
            LDR     R0, =SYSCTL_RCGCGPIO_R  		;Carrega o endereço do registrador RCGCGPIO
			MOV		R1, #GPIO_PORTN                 ;Seta o bit da porta N
			ORR     R1, #GPIO_PORTJ					;Seta o bit da porta M, fazendo com OR
            STR     R1, [R0]						;Move para a memória os bits das portas no endereço do RCGCGPIO
 
            LDR     R0, =SYSCTL_PRGPIO_R			;Carrega o endereço do PRGPIO para esperar os GPIO ficarem prontos
EsperaGPIO  LDR     R1, [R0]						;Lê da memória o conteúdo do endereço do registrador
			MOV     R2, #GPIO_PORTN                 ;Seta os bits correspondentes às portas para fazer a comparação
			ORR     R2, #GPIO_PORTJ                 ;Seta o bit da porta M, fazendo com OR
            TST     R1, R2							;ANDS de R1 com R2
            BEQ     EsperaGPIO					    ;Se o flag Z=1, volta para o laço. Senão continua executando
 
; 2. Limpar o AMSEL para desabilitar a analógica
            MOV     R1, #0x00						;Colocar 0 no registrador para desabilitar a função analógica
            LDR     R0, =GPIO_PORTJ_AHB_AMSEL_R     	;Carrega o R0 com o endereço do AMSEL para a porta K
            STR     R1, [R0]						;Guarda no registrador AMSEL da porta K da memória
            LDR     R0, =GPIO_PORTN_AMSEL_R		;Carrega o R0 com o endereço do AMSEL para a porta N
            STR     R1, [R0]					    ;Guarda no registrador AMSEL da porta N da memória
 
; 3. Limpar PCTL para selecionar o GPIO
            MOV     R1, #0x00					    ;Colocar 0 no registrador para selecionar o modo GPIO
			LDR     R0, =GPIO_PORTJ_AHB_PCTL_R			;Carrega o R0 com o endereço do PCTL para a porta M
            STR     R1, [R0]                        ;Guarda no registrador PCTL da porta M da memória
            LDR     R0, =GPIO_PORTN_PCTL_R      ;Carrega o R0 com o endereço do PCTL para a porta N
            STR     R1, [R0]                        ;Guarda no registrador PCTL da porta N da memória
; 4. DIR para 0 se for entrada, 1 se for saída
			LDR     R0, =GPIO_PORTJ_AHB_DIR_R		;Carrega o R0 com o endereço do DIR para a porta K
			MOV     R1, #2_00000000					
            STR     R1, [R0]						;Guarda no registrador			
			LDR     R0, =GPIO_PORTN_DIR_R		;Carrega o R0 com o endereço do DIR para a porta N
			MOV     R1, #2_00000010					;PN1 & PN0 para LED
            STR     R1, [R0]						;Guarda no registrador
					
; 5. Limpar os bits AFSEL para 0 para selecionar GPIO 
            MOV     R1, #0x00						;Colocar o valor 0 para não setar função alternativa
            LDR     R0, =GPIO_PORTJ_AHB_AFSEL_R     ;Carrega o endereço do AFSEL da porta K
            STR     R1, [R0]                        ;Escreve na porta	
            LDR     R0, =GPIO_PORTN_AFSEL_R		;Carrega o endereço do AFSEL da porta N
            STR     R1, [R0]						;Escreve na porta
; 6. Setar os bits de DEN para habilitar I/O digital
            LDR     R0, =GPIO_PORTJ_AHB_DEN_R			;Carrega o endereço do DEN
            LDR     R1, [R0]                            ;Ler da memória o registrador GPIO_PORTN_DEN_R
			MOV     R2, #2_00000011                           
            ORR     R1, R2                              
            STR     R1, [R0]                            ;Escreve no registrador da memória funcionalidade digital
			
            LDR     R0, =GPIO_PORTN_DEN_R			;Carrega o endereço do DEN
            LDR     R1, [R0]							;Ler da memória o registrador GPIO_PORTN_DEN_R
			MOV     R2, #2_00000010	
            ORR     R1, R2
            STR     R1, [R0]							;Escreve no registrador da memória funcionalidade digital 
         
; 7. Para habilitar resistor de pull-up interno, setar PUR para 1
			LDR     R0, =GPIO_PORTJ_AHB_PUR_R			;Carrega o endereço do PUR para a porta M
			MOV     R1, #2_00000011						;Habilitar funcionalidade digital de resistor de pull-up 
            STR     R1, [R0]							;Escreve no registrador da memória do resistor de pull-up

;Interrupções
; 8. Desabilitar a interrupção no registrador IM
			LDR     R0, =GPIO_PORTJ_AHB_IM_R			;Carrega o endereço do IM para a porta M
			MOV     R1, #2_00							;Desabilitar as interrupções  
            STR     R1, [R0]							;Escreve no registrador
            
; 9. Configurar o tipo de interrupção por borda no registrador IS
			LDR     R0, =GPIO_PORTJ_AHB_IS_R			;Carrega o endereço do IS para a porta M
			MOV     R1, #2_00							;Por Borda  
            STR     R1, [R0]							;Escreve no registrador

; 10. Configurar  borda única no registrador IBE
			LDR     R0, =GPIO_PORTJ_AHB_IBE_R				;Carrega o endereço do IBE para a porta M
			MOV     R1, #2_00							;Borda Única  
            STR     R1, [R0]							;Escreve no registrador
; 11. Configurar  borda de descida (botão pressionado) no registrador IEV
			LDR     R0, =GPIO_PORTJ_AHB_IEV_R				;Carrega o endereço do IEV para a porta M
			MOV     R1, #2_10							;Borda Única  
            STR     R1, [R0]
			;Escreve no registrador
; 12. Configurar  ICR
			LDR     R0, =GPIO_PORTJ_AHB_ICR_R				;Carrega o endereço do IEV para a porta M
			MOV     R1, #2_11							;Borda Única  
            STR     R1, [R0]
  
; 12. Habilitar a interrupção no registrador IM
			LDR     R0, =GPIO_PORTJ_AHB_IM_R				;Carrega o endereço do IM para a porta M
			MOV     R1, #2_11							;Habilitar as interrupções  
            STR     R1, [R0]							;Escreve no registrador
            
; 13. Habilitar a interrupção no NVIC
			LDR     R0, =NVIC_EN1_R           			;Carrega o do NVIC para o grupo que tem o M entre 64 e 95
			MOV     R1, #1
			LSL     R1, #19								;Desloca 8 bits para a esquerda já que o M é a interrupção do bit 8 no EN2
            STR     R1, [R0]							;Escreve no registrador da memória

; 13. Setar a prioridade no NVIC
			LDR     R0, =NVIC_PRI12_R           		;Carrega o do NVIC para o grupo que tem o M entre 72 e 75
			MOV     R1, #5  		                    ;Prioridade 3
			LSL     R1, #29						;Desloca 5 bits para a esquerda já que o M é o primeiro byte do PRI18
            STR     R1, [R0]
			BX  LR

; -------------------------------------------------------------------------------
; Função PortN_Output
; Parâmetro de entrada: R0 --> se os BIT1 está ligado ou desligado
; Parâmetro de saída: Não tem
PortN_Output
	LDR	R1, =GPIO_PORTN_DATA_R		    ;Carrega o valor do offset do data register
	;Read-Modify-Write para escrita
	LDR R2, [R1]
	BIC R2, #2_00000010                     ;Primeiro limpamos os dois bits do lido da porta R2 = R2 & 11111101
	ORR R0, R0, R2                          ;Fazer o OR do lido pela porta com o parâmetro de entrada
	STR R0, [R1]                            ;Escreve na porta N o barramento de dados dos pinos [N5-N0]
	BX LR									;Retorno5

; -------------------------------------------------------------------------------
; Função ISR GPIOPortM_Handler (Tratamento da interrupção)
GPIOPortJ_Handler
	LDR R0, =GPIO_PORTJ_AHB_RIS_R		;pega alguma interrupção
	LDR R1, [R0]						;
	CMP R1, #2_01						;compara para ver se é a chave 1 ou chave 2
	BNE Chave2
	LDR R0, =GPIO_PORTJ_AHB_ICR_R		;
	MOV R1, #2_01
	STR R1, [R0]
	PUSH {LR}
	MOV R0, #2_10				;acende o LED
	BL PortN_Output
	POP {LR}
	B Fim_Interrupt
	
Chave2
	LDR R0, =GPIO_PORTJ_AHB_ICR_R
	MOV R1, #2_10
	STR R1, [R0]
	PUSH {LR}
	BL Apaga
	POP {LR}

Fim_Interrupt
    BX LR             

Apaga
	MOV R0, #0
	PUSH {LR}
	BL PortN_Output
	POP {LR}
	BX LR
 
    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
        
        
        