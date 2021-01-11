;TITLE "Microprocessor hw 6" 
;USING SIMPLIFIED VERSION OF SEGMENTS
.MODEL SMALL                         ; small size program
.STACK 64                            ; stack size
.DATA                                ; data segment
	ARITH_ERR   db "Arithmetic Error. cannot divide number by 0$"
	OP_ERR      db "Operator not defined$"
    NEW_LINE    db 10, 13, '$'
    NUM1_MSG    db "num1 = $"
    NUM2_MSG    db "num2 = $"
    OP_MSG      db "operator = $"
    RESULT_MSG  db "Result = $"
    equation db 6 dup("$")
    NUM1    db ?,'$'
    NUM2    db ?,'$'
    OP	    db ?,'$'
    RESULT  dw ?,'$'
    DIGITS  db 6 dup("$")
.CODE                               ; code segment
MAIN PROC FAR                       ; program entry point
    MOV AX,@DATA                    ; load data segment address
    MOV DS,AX                       ; assign value to ds
    CLC                             ; clear carry
;---------------------
    MOV SI,OFFSET equation          ; move start offset of equation to SI
INPUT:                              ; read input equation
 	MOV AH,01H                      ; read character function
 	INT 21H                         ; interrupt, save result to AL
 	CMP AL, 13                      ; if input character is Enter
 	JE  PROCESS                     ; go to process
 	MOV [SI],AL                     ; save character in equation with SI offset
 	INC SI                          ; increase one byte
 	JMP INPUT                       ; go for reading next character

PROCESS:                            ; separate num1,num2 and operator
 	DEC SI
 	MOV AL, [SI]
 	MOV NUM2,AL                     ; num 2 right digit
 	SUB NUM2, '0'

 	DEC SI
 	MOV AL, [SI]
 	CMP AL, 48                      ; compare to 0 ascii character dec for zero = 48
    JB  OPERAND                     ; if not number jump to operand
    SUB AL, '0'
    MOV BL, 10                      ; multiply AL to 10
    MUL BL                          ; do the multiplication
    ADD NUM2, AL                    ; ADD AL*10 to num1 and save to num1

    DEC SI
    MOV AL, [SI]
OPERAND:                            ; save operand
    MOV OP, AL

    DEC SI
    MOV AL, [SI]
    MOV NUM1, AL
    SUB NUM1, '0'


    CMP SI, OFFSET equation         ; if SI =start offset of equation -> start calculation
    JBE CALCULATION
                                    ; one digit left
    DEC SI
    MOV AL, [SI]
    SUB AL, '0'
    MOV BL, 10
    MUL BL
    ADD NUM1, AL
                                    ;   OPERATOR    |   HEX CODE
                                    ;      *        |      2A
                                    ;      +        |      2B
                                    ;      -        |      2D
                                    ;      /        |      2F
CALCULATION:
    MOV AL, op                      ; load operator

MULTIPLY:
    CMP AL, 2AH                     ; if operator is *
    JA SUMMATION                    ; if not go to next
    MOV AL, NUM1
    MOV BL, NUM2
    MUL BL
    MOV RESULT, AX
    JMP PRINT_RESULT
SUMMATION:
    CMP AL, 2BH                     ; if operator is +
    JA SUBTRACTION                  ; if not go to next
    MOV AL, NUM1
    ADD AL, NUM2
    MOV AH, 0
    MOV RESULT, AX
    JMP PRINT_RESULT
SUBTRACTION:
    CMP AL, 2DH                     ; if operator is -
    JA DIVISION                     ; if not go to next
    MOV AL, NUM1
    MOV AH, NUM2

    CMP AL, AH
    JB SUB_SF                       ; if num1 < num2 go to SUB_SF
    SUB AL,AH                       ; first - second
    JMP SUB_SAVE
SUB_SF:                             ; second - first
    SUB AH, AL
    MOV AL, AH
SUB_SAVE:
    MOV AH, 0
    MOV RESULT, AX
    JMP PRINT_RESULT
DIVISION:
    CMP AL, 2FH
    JA PRINT_OP_ERR                 ; jump to PRINT_OP_ERR if operator is not division
    JB PRINT_OP_ERR
    MOV BL, NUM2
    CMP BL, 0
    JBE PRINT_ARITH_ERR             ; if second is zero print err
    MOV AL, NUM1
    SUB AH, AH
    DIV NUM2
    MOV AH, 0                       ; dont care about remainder
    MOV RESULT, AX
    JMP PRINT_RESULT

                                    ; print arithmetic error
PRINT_ARITH_ERR:
    MOV DX,OFFSET ARITH_ERR         ; move start offset of arthimetic error in dx
    CALL PRINTLN
    JMP PRINT_DATA
                                    ; print invalid operator error
PRINT_OP_ERR:
	MOV DX,OFFSET OP_ERR            ; move start offset of invalid operator error in dx
	CALL PRINTLN
    JMP PRINT_DATA
                                    ; print result of calculation
PRINT_RESULT:
	MOV DX,OFFSET RESULT_MSG
    CALL PRINT

    MOV BL, 0                       ; default assume result is positive
    MOV AL,OP                       ; if operator is subtraction -> check if result is negative
    CMP AL, 2DH
    JA NOT_NEG
    JB NOT_NEG

    MOV AL, NUM1
    MOV AH, NUM2
    CMP AL, AH
    JAE NOT_NEG                     ; IF NUM1 >= NUM2 go to NOT_NEG
    MOV BL, 1                       ; set result as negative
NOT_NEG:
    MOV AX, RESULT
    CALL TO_BCD                     ; convert result to separate bcd values in DIGITS

  	MOV DX,OFFSET DIGITS
    CALL PRINTLN

PRINT_DATA:
                                    ; print number 1
    MOV DX, OFFSET NUM1_MSG
    CALL PRINT

    MOV AL, NUM1
    MOV AH, 0
    MOV BL, 0                       ; pos number
    CALL TO_BCD

  	MOV DX,OFFSET DIGITS
    CALL PRINTLN

                                    ; print number 2
    MOV DX, OFFSET NUM2_MSG
    CALL PRINT

    MOV AL, NUM2
    MOV AH, 0
    MOV BL, 0                       ; pos number
    CALL TO_BCD

  	MOV DX,OFFSET DIGITS
    CALL PRINTLN

                                    ; print operator
    MOV DX, OFFSET OP_MSG
    CALL PRINT
  	MOV DX,OFFSET OP
    CALL PRINTLN

                                    ; Reutrn to dosbox
    MOV AH,4CH
    INT 21H

MAIN ENDP
;================= print function ====================== DX hold data offset to print
PRINT PROC NEAR
    MOV AH,09H
    INT 21H
    RET
PRINT ENDP
;================= println function ====================== DX hold data offset to print
PRINTLN PROC NEAR
  	MOV AH,09H
  	INT 21H
  	MOV DX,OFFSET NEW_LINE
  	MOV AH,09H
  	INT 21H
  	RET
PRINTLN ENDP
;================= number to bcd function ====================== AX hold number, BL hold sign -> pos(0) or neg(1)
TO_BCD PROC NEAR
    MOV SI, OFFSET DIGITS
    MOV CX, 6                       ; DIGITS length
RESET:                              ; reset DIGITS in memory
    MOV BYTE PTR [SI], '$'
    INC SI
    LOOP RESET

    MOV SI, OFFSET DIGITS
    CMP BL, 0
    JBE INIT                        ; go to INIT if number is positive
    MOV BYTE PTR [SI], '-'          ; negative sign - decimal value = 45
    INC SI
INIT:
    MOV BX, 10
    MOV CX, 0
                                    ; divide number to 10
                                    ; put the remainder in stack
                                    ; repeat until number become 0
BCD_LOOP:
    SUB DX, DX
    DIV BX
    PUSH DX
    INC CX                          ; increament counter register on each push
    CMP AX,0
    JA BCD_LOOP
WRITE_DIGIT:                        ; write to DIGITS in memory
    POP AX
    ADD AL, '0'
    MOV [SI], AL
    INC SI
    LOOP WRITE_DIGIT                ; continue until counter register become 0
    RET
TO_BCD ENDP
;=======================================================
END MAIN