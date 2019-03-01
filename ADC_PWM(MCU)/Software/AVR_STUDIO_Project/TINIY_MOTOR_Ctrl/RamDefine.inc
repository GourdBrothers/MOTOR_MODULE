;============================================
;*
;*	RamDefine.inc
;*
;============================================

#ifndef _RamDefine_INC_
#define _RamDefine_INC_


.def		REG0	=	R16
.def		REG1	=	R17
.def		REG2	=	R18

.DSEG		
.ORG		0x60

SysFlow:	.BYTE	1
SysFlag:	.BYTE	1


#endif  /* _TN13DEF_INC_ */



