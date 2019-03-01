;================================================
;===
;===  AVR_MACRO.asm
;===
;================================================

MOVFL   MACRO   F1,D
  LDI   R16,D
  STS   F1 ,R16
  ENDM
  
  
MOVFF   MACRO   F1,F2
  LDS   R16,F2
  STS   F1 ,R16
  ENDM
  