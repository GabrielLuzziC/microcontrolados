; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
VETOR_ENTRADA EQU 0x20000400 
VETOR_SAIDA	EQU	0x20000600
N_ELEMENTOS EQU 30
RAZAO	EQU 0x20000700
; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM	

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

; -------------------------------------------------------------------------------
; Função main()
Start  
; Comece o código aqui <======================================================
	BL bubble_sort
	
	BL pg
	LDR R5, =VETOR_SAIDA
	BL print
	
; BUBBLE SORT	
bubble_sort
	LDR R0, =VETOR_ENTRADA
	MOV R1, #N_ELEMENTOS
	SUB R1, #1
	MOV R2, #0  ; indice i 0 até n-2
	
loopExt
	MOV R3, #0	; indice j 0 ate n-2-i
	MOV R6, R1
	SUB R6, R2
	MOV R4, #0
	
loopInt
	LDRH R8, [R0, R4] ; elemento atual
	ADD R5, R4, #2	; j+1
	LDRH R9, [R0, R5] ; proximo elemento
	
	CMP R8, R9 
	BLS nao_troca
	
	; troca se for maior
	STRH R9, [R0, R4]
	STRH R8, [R0, R5]
	
nao_troca
	ADD R4, #2 ; prox posição da memória
	ADD R3, #1
	
	CMP R3, R6
	BNE loopInt
	
	ADD R2, #1
	CMP R2, R1
	BNE loopExt
	
	BX LR  ; retorno da função
	
; ---------------------------------------------------------
; PG	

pg
	LDR R0, =VETOR_ENTRADA  ; endereço de memória do inicio
	MOV R3, R0
loopExt_pg
	LDRH R6, [R0]	; le elemento atual em R6
	ADD R0, #2
	LDR R3, =VETOR_ENTRADA  ; endereço de memória do inicio
loopInt_pg
	; achar a razao para esse termo a com o resto do vetor
	ADD R3, #2
	LDRH R7, [R3]	; a + 1
	CMP R7, #0
	BEQ loopExt_pg
	UDIV R8, R7, R6 ; calcula a razao (R8)
	MLS R9, R8, R6, R7  ; se for 0 é divisivel
	
	PUSH {R6}
	PUSH {R7}
	MOV R1, #2	; dois elementos (contador elementos PG)
	MOV R2, R3
	CMP R9, #0
	BEQ prox_termo
	
	; se n pulou tira da pilha
	POP	{R4} ; lixo
	POP {R4} ; lixo
	MOV R1, #0
	
	;ADD R3, #2 
	
	B loopInt_pg
	
	
prox_termo
	; R8 é a razao e R7 o 2º termo
	MUL R10, R8, R7
	ADD R2, #2		; prox termo
	
	LDRH R12, [R2]
	CMP R12, #0
	BEQ limpa_pilha ; execedeu o limite do vetor, procura prox razao
	
	CMP R12, R10 ; se achou o prox termo add na pilha
	BEQ add_pilha

	BMI prox_termo
	
	;ADD R1, #2
	BPL limpa_pilha
	;BPL loopInt_pg

add_pilha
	ADD R1, #1 ; num de elementos
	PUSH {R12}
	MOV R7, R10 ; 
	
	CMP R1, #7
	BNE prox_termo
	
	BX LR ; retorno da função
	
limpa_pilha
	
	POP {R4}
	SUB R1, #1
	
	CMP R1, #0
	BNE limpa_pilha
	
	B loopInt_pg
	
	
	
; ---------------------------------------------

print 
	POP {R4}
	STRH R4, [R5, #2]!
	SUB R1, #1
	CMP R1, #0
	BNE print 
	

	
;	LDR R0, =VETOR_ENTRADA
;	MOV R5, #N_ELEMENTOS
;	MOV R6, #0 ; iterator
;	MOV R1, R0
;	LDRH R7, R0
;	B loop_int_pg
;loop_ext_pg	
;	ADD R0, #2  ; prox posicao na mem
;	MOV R1, R0
;	MOV R6, #0 ; iterator
;	LDRH R7, R0
;loop_int_pg
;	ADD R1, R1, #2 	; next word
;	LDRH R8, R1
;	ADD R6, #1
;	CMP R5, R6
;	BEQ loop_ext_pg
;	UDIV R2, R7, R0
;	MLS R3, R2, R0, R7
;	CMP R3, #0
;	BEQ equal		
;dif
;	B loop_int_pg
;equal	; (encontrou um par) R2 = razao
;	ADD R9, R6
;	MUL R4, R2, R1  ; target: prox termo da pg
;	ADD R9, #1
;	CMP R5, R6
;	BEQ loop_ext_pg
;	ADD R1, R1, #2 ; next word
;	LDRH R8, R1
;	CMP R4, R8
;	BMI loop_int_pg; se R8 > R4
	
	
	NOP								; necessário para funcionar
    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo