	; Aliases
	ALIAS R4, RIGHT
	ALIAS R5, LEFT
	ALIAS R10, RIGHT_VALUE
	ALIAS R11, LEFT_VALUE
	
	ALIAS R3, ELEM_SIZE
	ALIAS R8, INIT_ELEM_ADDR
	ALIAS R9, ELEMS_ADDR
	
	; Areas
	AREA SOTRAGE, DATA, READONLY, ALIGN=4
NUMELEMS	DCD			10
INITELEMS	DCD			9,8,7,6,5,4,3,2,1,0	
	AREA ARRAY_DATA, DATA, READWRITE
ELEMS		DCD			0,0,0,0,0,0,0,0,0,0
	AREA RESET, CODE, READONLY
	ENTRY
;============================= Init Elems
	LDR INIT_ELEM_ADDR, =INITELEMS
	LDR ELEM_SIZE, NUMELEMS
	LDR ELEMS_ADDR, =ELEMS
	MOV R2, #0
INIT_ELEMS
	CMP R2, ELEM_SIZE
	BEQ END_ELEMS
	LDR R0, [INIT_ELEM_ADDR, R2, LSL #2]
	STR R0, [ELEMS_ADDR, R2, LSL #2]
	ADDS R2, #1
	B INIT_ELEMS
END_ELEMS

;=============================	Main
	MOV RIGHT, #0
	MOV R0, ELEM_SIZE 
	MOV R1, ELEMS_ADDR
	BL INSSORT
	B END_PROGRAM
	
;=============================	
	
INSSORT
	CMP R0, RIGHT ; for condition
	BEQ BACK_FROM_INSSORT ; break instruction
	
	LDR RIGHT_VALUE, [R1, RIGHT, LSL #2] ; rightVal = arr[right];
	SUB LEFT, RIGHT, #1 ; left = right - 1; 
	
	SUB SP, #4
	STR LR, [SP]
	BL WHILE_LOOP ; inner while loop
	
	ADDS LEFT,#1 ; left + 1 
	STR RIGHT_VALUE, [R1, LEFT, LSL #2] ; arr[left + 1] = rightVal; 
	
	ADDS RIGHT, #1 ; i++
	B INSSORT ; loop through for again
;=============================	Inner While Loop
WHILE_LOOP
	LDR LEFT_VALUE, [R1, LEFT, LSL #2] ; arr[left]
	CMP LEFT, #0
	BLT BACK_FROM_ROUTINE
	CMP LEFT_VALUE, RIGHT_VALUE
	BLE BACK_FROM_ROUTINE
	; lines 35 to 39 equasl to -> break if not left >= 0 && arr[left] > rightVal
	
	ADD LEFT, LEFT, #1 ; line ##### equals to -> arr[left + 1] = arr[left]; 
	STR LEFT_VALUE, [R1, LEFT, LSL #2]
	
	; the reason of subtracting 2 is that line 38 add 1 to left
	SUBS LEFT, LEFT, #2 ; equals to -> left--
	
	B WHILE_LOOP
;=============================	
BACK_FROM_ROUTINE
	BX LR 
BACK_FROM_INSSORT
	LDR LR, [SP]
	ADDS SP, #4
	MOV PC, LR
	

;=============================	
END_PROGRAM	

	END