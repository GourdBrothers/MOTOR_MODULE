;=============================================
;==     
;==  Fun_ADC.asm
;==
;=============================================


  #include      "RamDefine.inc"
  #include      "AVR_MACRO.asm"
;===
  NAME          Fun_FOR_ADC
;===
  PUBLIC        Fun_ADC_Close
  PUBLIC        Fun_ADC_init
;=== 
  RSEG          CODE
  
Fun_ADC_init:
  MOVFL         Buffer0,0x56
  MOVFL         Buffer1,0xaa
  MOVFL         Buffer2,0x66
  ret
  
Fun_ADC_Close:
  MOVFF         Buffer0,Buffer1
  MOVFF         Buffer1,Buffer2
  MOVFL         Buffer2,99h
  ret
  
  end
