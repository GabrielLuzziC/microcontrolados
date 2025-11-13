; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Laboratório 1
; Alunos: Gabriel Luzzi Correa & Thomas Pinton
; Template do Prof. Guilherme Peron
; 23/09/2025

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------

; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		

; Definição de cada dígito para display 7 seguimentos
ZERO_7SEG    EQU 2_0111111   ; a b c d e f acesos
UM_7SEG      EQU 2_0000110   ; b c
DOIS_7SEG    EQU 2_1011011   ; a b d e g
TRES_7SEG    EQU 2_1001111   ; a b c d g
QUATRO_7SEG  EQU 2_1100110   ; b c f g
CINCO_7SEG   EQU 2_1101101   ; a c d f g
SEIS_7SEG    EQU 2_1111101   ; a c d e f g
SETE_7SEG    EQU 2_0000111   ; a b c
OITO_7SEG    EQU 2_1111111   ; todos
NOVE_7SEG    EQU 2_1101111   ; a b c d f g
TEMPO		 EQU 167		 ; tempo para incrementar temp

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms										
		IMPORT  GPIO_Init
        IMPORT  PortN_Output
		IMPORT  PortA_Output
        IMPORT  PortQ_Output
		IMPORT  PortP_Output_Q1
		IMPORT  PortB_Output_Q2
		IMPORT  PortB_Output_Q3
		IMPORT  GPIOPortJ_Handler

; -------------------------------------------------------------------------------
; Função main()
Start  		
	BL PLL_Init                  	;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init              	;Chama a subrotina para inicializar o SysTick
	BL GPIO_Init                 	;Chama a subrotina que inicializa os GPIO
	
	;Inicializa as temperaturas
	MOV R3, #15					 	;R3 = Temperatura atual, inicilizada em 15º
	MOV R9, #25					 	;R9 = Temperatura setpoint, inicalizada em 25º
	MOV R0, #0
	;Desativa todos transistores
	PUSH{LR}
	BL PortP_Output_Q1
	BL PortB_Output_Q2
	BL PortB_Output_Q3
	POP{LR}
	;Inializa temporizador 1seg
    MOV R5, #0
	LDR R8, =TEMPO
	
;Loop principal
Temporizador1Seg
    BL VerificaLeds
    ADD R5, R5, #1
    CMP R5, R8 						; 1000ms / 6ms = aprox 167
    BNE Temporizador1Seg

;Incrementa ou decrementa temperatura atual
VerificarTemperatura
    MOV R5, #0						; Zera temporizador
    CMP R3, R9
	ADDLT R3, #1					; R3 < R9, atual < setpoint
	SUBGT R3, #1					; R3 > R9, atual > setpoint                
	B Temporizador1Seg

;Verifica todos os LEDs e displays
VerificaLeds
	PUSH{LR}
	BL AcendeLeds_PAT				; Mostra temperatura de setpoint
	BL AcendeDisplay1				; Display 1
	BL AcendeDisplay2				; Display 2
	CMP R3, R9
	BGT AcendeLed_PN1				; R3 > R9, atual > setpoint
	BLT AcendeLed_PN0				; R3 < R9, atual < setpoint
	BEQ AcendeAmbos					; R3 = R9, atual = setpoint
FimVerifica
	POP{LR}
	BX LR
	
; Acende LED PN0
AcendeLed_PN0
	MOV R0, #2_01
	PUSH {LR}
	BL PortN_Output
	POP {LR}
	B FimVerifica	
	
; Acende LED PN1
AcendeLed_PN1
	MOV R0, #2_10
	PUSH {LR}
	BL PortN_Output
	POP {LR}
	B FimVerifica
	
; Acende LED PN0 & PN1
AcendeAmbos
	MOV R0, #2_11
	PUSH {LR}
	BL PortN_Output
	POP {LR}
	B FimVerifica
	
; Acende display 1
AcendeDisplay1
	MOV R7, #10
    UDIV R6, R3, R7       			 ; R6 = dezena
	PUSH{LR}
	BL pega_7seg
	BL PortA_Output
	BL PortQ_Output
	MOV R0, #2_10000					
	BL PortB_Output_Q2
	MOV R0, #1
	BL SysTick_Wait1ms
	MOV R0, #0
	BL PortB_Output_Q2
	MOV R0, #1
	BL SysTick_Wait1ms
	POP{LR}
	BX LR

; Acende display 2
AcendeDisplay2
	MOV R7, #10
	UDIV R6, R3, R7
    MLS R0, R6, R7, R3   			 ; R7 = unidade 
	MOV R6, R0
	PUSH{LR}
	BL pega_7seg
	BL PortA_Output
	BL PortQ_Output
	MOV R0, #2_100000					
	BL PortB_Output_Q3
	MOV R0, #1
	BL SysTick_Wait1ms
	MOV R0, #0
	BL PortB_Output_Q3
	MOV R0, #1
	BL SysTick_Wait1ms
	POP{LR}
	BX LR

; Acende LEDs da placa PAT
AcendeLeds_PAT
	MOV R0, R9						  ;Bits de R9 vão para R0
	PUSH {LR}
	BL PortA_Output
	BL PortQ_Output
	MOV R0, #2_100000				  ;Port P5
	BL PortP_Output_Q1
	MOV R0, #1
	BL SysTick_Wait1ms
	MOV R0, #0
	BL PortP_Output_Q1
	MOV R0, #1
	BL SysTick_Wait1ms
	POP {LR}
	BX LR
	
; Retorna os bits de 7Seg em RO
pega_7seg
	CMP R6, #0 
	ITT EQ
		LDREQ R0, =ZERO_7SEG
		BEQ fim_pega_7seg
	CMP R6, #1 
	ITT EQ
		LDREQ R0, =UM_7SEG
		BEQ fim_pega_7seg
	CMP R6, #2 
	ITT EQ
		LDREQ R0, =DOIS_7SEG
		BEQ fim_pega_7seg
	CMP R6, #3 
	ITT EQ
		LDREQ R0, =TRES_7SEG
		BEQ fim_pega_7seg
	CMP R6, #4 
	ITT EQ
		LDREQ R0, =QUATRO_7SEG
		BEQ fim_pega_7seg
	CMP R6, #5 
	ITT EQ
		LDREQ R0, =CINCO_7SEG
		BEQ fim_pega_7seg
	CMP R6, #6 
	ITT EQ
		LDREQ R0, =SEIS_7SEG
		BEQ fim_pega_7seg
	CMP R6, #7 
	ITT EQ
		LDREQ R0, =SETE_7SEG
		BEQ fim_pega_7seg
	CMP R6, #8 
	ITT EQ
		LDREQ R0, =OITO_7SEG
		BEQ fim_pega_7seg
	CMP R6, #9 
	ITT EQ
		LDREQ R0, =NOVE_7SEG
		BEQ fim_pega_7seg
fim_pega_7seg
	BX LR

; -------------------------------------------------------------------------------
; FIM
; -------------------------------------------------------------------------------
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo
