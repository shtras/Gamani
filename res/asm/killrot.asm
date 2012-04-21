		jmp		main

sens:	mov r1, 0x9500	; input address
		movs r2, 0x1	; output address
		fload f0, [r1]	; load yaw velocity
		fcmp f0, 0
		jg gt
		je eq
						; yaw velocity == 0
		fmov f1, 0
		fadd f1, f0
		jmp set
gt:		fmov f1, f0		; yaw velocity > 0
		jmp set
eq:		fmov f1, 0		; yaw velocity < 0
set:	fstore [r2], f1	; store output
		ret
		
main:	mov		r1, 0x9000
		mov 	[r1], sens	; store address of interrupt routine into interrupt vector
wait:	jmp 	wait		; busy wait


;		interrupt vector:
;		sensor update: 0x9000
;		data vector:
;		0x9500: 64 bit yaw velocity
;		output vector:
;		0x10000: 64bit yaw power [0, 1]
