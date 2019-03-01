;
; FunLib.asm
; 
;

Fun_Delay10ms:
	LDI		REG1,10
	RJMP	Fun_Delay_ms

Fun_Delay_1ms:
	LDI		REG1,1

Fun_Delay_ms:
	NOP
Fun_Delay_msLoop:
;---	
	LDI		REG0,100
Fun_Delay_0_Loop:
	NOP
	NOP
	NOP
	DEC		REG0
	BRNE	Fun_Delay_0_Loop
Fun_Delay_0_END:
;---	
	DEC		REG1
	BRNE	Fun_Delay_msLoop
RET


Fun_Sys_init:
	LDI		R16	,0x80
	STS		CLKPR,R16
RET




