;====================================================
		.include	<tn13def.inc>
		.include	"RamDefine.inc"
;====================================================
		.CSEG
		.ORG		0x00
		RJMP		SysReset
		RJMP		SysISR_INT0
		RJMP		SysISR_PCI0
		RJMP		SysISR_OVF0
		RJMP		SysISR_ERDY
		RJMP		SysISR_ACI
		RJMP		SysISR_OC0A
		RJMP		SysISR_OC0B
		RJMP		SysISR_WDT
		RJMP		SysISR_ADCC

;-----------------------------------------------------
		.include	"SysISR_Handle.asm"
		.include	"FunLib.asm"

;-----------------------------------------------------		
SysReset:	
		RCALL		Fun_Delay_ms
		RCALL		Fun_Delay10ms

;-----------------------------------------------------
main:
		
		RJMP		main

;-----------------------------------------------------
		.ESEG

SN:		.DW	0x1234


;-----------------------------------------------------	
		.EXIT

;-----------------------------------------------------
