	.MODEL SMALL
	.STACK	64
	.DATA
		PRIME_SIZE       DB 00H
		NUMBER 		  		DB   ?
		CUR_NUM    	    DW ?		
		PORTA 		        EQU 0C8H
		PORTB 		        EQU 0CAH
		PORTC 		        EQU 0CCH
		PORT_CON   	    EQU 0CEH
		TIM_PORT_CON EQU 0CFH
		TIM_PORT_A       EQU 0C9H
		TIMER_WR         EQU 0E0H
		TIMER_CON  		EQU 0E3H
		MEM1ADR   		    EQU 2800H
		MEM2ADR   	   	    EQU 2900H
	.CODE	
MAIN PROC	 FAR
		MOV		AX, @DATA
		MOV		DS, AX
;//////////////////////////////////////////KEYPAD//////////////////////////////////////
		;INPUT NUMBER SAVE TO BL
		MOV DX, PORT_CON
		MOV AL, 10000010B ;PORT A , C OUTPUT AND PORT B INPUT
		OUT DX, AL
		;RESULT
		MOV BX, 00H
		;READ 2 DIGITS
		MOV CX, 02H
FIRST: 
		CMP CX, 0
		 JZ OUTKEYPAD
;//////////////////////////////////////////ONE//////////////////////////////////////
ONE :	; FIRST ROW OF KEYPAD
			MOV DX, PORTC
			MOV AL, 11111110B
			OUT DX, AL
			; TO GET CHOSEN NUMBER FROM USER
			MOV DX, PORTB
			IN AL, DX
			MOV AH, AL
			CMP AH, 11111011B
			JNZ TWO
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 01H
			DEC CX
;//////////////////////////////////////////TWO//////////////////////////////////////
TWO :	CMP AH, 11111101B
			JNZ THREE
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 02H
			DEC CX
;//////////////////////////////////////////THREE//////////////////////////////////////
THREE :	CMP AH, 11111110B
			JNZ FOUR
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 03H
			DEC CX
;//////////////////////////////////////////FOUR//////////////////////////////////////
FOUR :	 ; SECOND ROW OF KEYPAD
			MOV DX, PORTC
			MOV AL, 11111101B
			OUT DX, AL
			; TO GET CHOSEN NUMBER FROM USER
			MOV DX, PORTB
			IN AL, DX
			MOV AH, AL
			CMP AH, 11111011B
			JNZ FIVE
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 04H
			DEC CX
;//////////////////////////////////////////FIVE//////////////////////////////////////
FIVE :	CMP AH, 11111101B
			JNZ SIX
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 05H
			DEC CX
;//////////////////////////////////////////SIX//////////////////////////////////////
SIX :	CMP AH, 11111110B
			JNZ SEVEN
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 06H
			DEC CX
;//////////////////////////////////////////SEVEN//////////////////////////////////////
SEVEN :	; THIRD ROW OF KEYPAD
			MOV DX, PORTC
			MOV AL, 11111011B
			OUT DX, AL
			; TO GET CHOSEN NUMBER FROM USER
			MOV DX, PORTB
			IN AL, DX
			MOV AH, AL
			CMP AH, 11111011B
			JNZ EIGHT
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 07H
			DEC CX
;//////////////////////////////////////////EIGHT//////////////////////////////////////
EIGHT : CMP AH, 11111101B
			JNZ NINE
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 08H
			DEC CX
;//////////////////////////////////////////NINE//////////////////////////////////////
NINE: CMP AH, 11111110B
			JNZ ZERO
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 09H
			DEC CX
;//////////////////////////////////////////ZERO//////////////////////////////////////
ZERO :	; FOURTH ROW OF KEYPAD
			MOV DX, PORTC
			MOV AL, 11110111B
			OUT DX, AL
			; TO GET CHOSEN NUMBER FROM USER
			MOV DX, PORTB
			IN AL, DX
			MOV AH, AL
			CMP AH, 11111101B
			JNZ FIRST
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			SHL BL, 1
			ADD BL, 00H	
			DEC CX
			JMP FIRST

OUTKEYPAD:
;//////////////////////////////////////////END_KEYPAD//////////////////////////////////////
		;KEEP INPUT IN NUMBER 
		MOV NUMBER, BL
;//////////////////////////////////////////PRIME//////////////////////////////////////
		;CONVERT INPUT TO HEX SAVE IN CL
		MOV CL, BL
		CALL CONVERT_DECTOHEX
		MOV CL, BL
		
		;FIND PRIME NUMBER LOWER THAN INPUT
		MOV CH, 00H    ;HEX
		MOV CUR_NUM, 00H     ;DECIMAL
		MOV SI, 00H		;RESULT SUM IN SI
		MOV DI, 00H  ;MEMORY ADDRES
FIND_PRIME_NUMBERS:		
		CALL PRIME_CHECK     ;RESULT IN AL : 1 IS PRIME AND 0 IS NOT PRIME
		CMP AL, 01H
		JNE NEXT_NUMBER
		;NUMBER IS PRIME AND SAVE IT
		MOV BX, CUR_NUM
		;SAVE IT IN MEMORY 1
		CALL SET_DS_MEM1
		MOV [DI], BX
		CALL SET_DS_DEFULT
		ADD DI, 02H
		;SUM PRIME NUMBER WITH RESULT
		MOV AX, SI
		ADD AX, CUR_NUM
		DAA
		MOV SI, AX
		;SAVE NUMBER OF PRIMES
		MOV AH, PRIME_SIZE
		INC AH
		MOV PRIME_SIZE, AH
NEXT_NUMBER:
		;INC DECIMAL
		MOV AX, CUR_NUM
		ADD AL, 01H
		DAA	
		MOV CUR_NUM, AX
		;INC HEX
		INC CH
		CMP CH, CL
		JNE FIND_PRIME_NUMBERS
;//////////////////////////////////////////END_PRIME/////////////////////////////////////
		;ADDRESS TO SAVE PRESULT SUM IN MEMORY2
		CALL SET_DS_MEM2
		;SAVE SUM RESULT
		MOV DI, 0000H 
		MOV [DI], SI
		CALL SET_DS_DEFULT
;//////////////////////////////////////////SHOW_MODE/////////////////////////////////////
		MOV DI, 0000H 
		MOV CL, PRIME_SIZE
		;TIMER WRITE SETTING
		MOV AL, 00011100B ;CONFIG MODE:COUNTER0/READ,LOAD LSB /MODE 2/BINARY CONTER
		OUT TIMER_CON, AL
		;TIMER READ SETTING
		MOV AL, 10010000B ;PORT A INPUT
		OUT TIM_PORT_CON, AL
		;ADDRESS TO SAVE PRIME NUMBERS MEMORY1
		CALL SET_DS_MEM1
SHOW_LOOP:
		MOV AX, [DI]
		OUT PORTA, AL
		;WRITE START VALUE IN TIMER 
		MOV AL, 32 ;LOWER BYTE
		OUT TIMER_WR, AL
		;READ FROM TIMER
READ_TIMER:
		;IN AL, TIM_PORT_A
		;CMP AL, 00H
		;JNE READ_TIMER
		CALL DELAY
		ADD DI, 02H
		DEC CL
		CMP CL, 00H
		JNE SHOW_LOOP
;//////////////////////////////////////////END_SHOW_MODE/////////////////////////////////////		
		CALL SET_DS_DEFULT

END_PROGRAM:
		JMP END_PROGRAM

MAIN ENDP
;//////////////////////////////////////////PRIME_CHECK//////////////////////////////////////
PRIME_CHECK PROC FAR     ;INPUT IN CH, RESULT IN AL
START: 
		MOV AL, CH
		;ONE IS NOT A PRIME NUMBER
		CMP AL , 01H
		JE FALSE
		;TWO IS A PRIME NUMBER
		CMP AL , 02H
		JE TRUE

		MOV BL,02H  ; START FROM 2
		MOV DX,0000H  
		MOV AH,00H   
		;LOOP TO CHECK FOR PRIME NUMBER
PRIME_LOOP:
		DIV BL
		CMP AH,00H  ;CHECK IF REMINDER IS 0 NUMBER IS NOT PRIME
		JNE NEXT
		JMP FALSE   
NEXT:
		INC BL         
		MOV AX,0000H   
		MOV DX,0000H   
		MOV AL,CH     
		CMP BL,CH  ;NUMBER IS PRIME IF BL = CH
		JNE PRIME_LOOP  
;THE GIVEN NUMBER IS PRIME
TRUE: 
		MOV AL, 01H
		RET
;THE GIVEN NUMBER IS NOT PRIME
FALSE: 
		MOV AL, 00H
		RET
PRIME_CHECK ENDP
;//////////////////////////////////////////END_PRIME_CHECK//////////////////////////////////////
;//////////////////////////////////////////CONVERT_DEC2HEX/////////////////////////////////////
CONVERT_DECTOHEX PROC FAR       ; INPUT IN CL AND RESULT IN BL
		MOV AL, CL
		MOV BL, 00  
		CMP AL, 16H
		JB EXIT
CONTINUE: 
		SUB AL, 16H 
		DAS 
		INC BL 
		CMP AL, 16H 
		JB EXIT
		JMP CONTINUE
EXIT:  
		CMP BL, 10H
		JB DIGIT2NOLETTER  
		SUB BL, 6 
DIGIT2NOLETTER:
		CMP AL, 10H
		JB DIGIT1NOLETTER  
		SUB AL, 6  
DIGIT1NOLETTER:     
		SHL BL, 1
		SHL BL, 1
		SHL BL, 1
		SHL BL, 1
		ADD BL, AL
		RET
CONVERT_DECTOHEX ENDP
;///////////////////////////////////END_CONVERT_DEC2HEX////////////////////////////////////
;////////////////////////////////////////////////////SET_DS_MEM1//////////////////////////////////////
SET_DS_MEM1 PROC FAR     
		MOV		AX, MEM1ADR
		MOV		DS, AX
		RET
SET_DS_MEM1 ENDP
;///////////////////////////////////END_SET_DS_MEM1////////////////////////////////////////
;////////////////////////////////////////////////////SET_DS_MEM2////////////////////////////////
SET_DS_MEM2 PROC FAR
		MOV		AX, MEM2ADR
		MOV		DS, AX
		RET
SET_DS_MEM2 ENDP
;///////////////////////////////////END_SET_DS_MEM2////////////////////////////////////////
;////////////////////////////////////////////////////SET_DS_DEFULT//////////////////////////////
SET_DS_DEFULT PROC FAR     
		MOV		AX, @DATA
		MOV		DS, AX
		RET
SET_DS_DEFULT ENDP
;///////////////////////////////////END_SET_DS_DEFULT/////////////////////////////////////
;////////////////////////////////////////////////////DELAY///////////////////////////////////////////////
DELAY PROC FAR     
		MOV BX, 30
DELAY_LOOP:
		DEC BX
		CMP BX, 0
		JNE DELAY_LOOP
		RET
DELAY ENDP
;///////////////////////////////////END_DELAY////////////////////////////////////////
		END MAIN