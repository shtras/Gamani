		jmp		main

func:	mov		r2, 10
loop:	sub		r2, 1
		cmp		r2, 0
		jg		loop
		ret

main:	movs	r1, 0x10
		add 	r1, 0x2000
		fload	f0, [r1]
		fadd	f0, 3.5e10
		fmov	f1, 1e10
		fsub	f0, f1
		fstore	0x10[r1], f0
		call	func
		mov 	r3, 5
		fmov	f2, 0.5
loop1:	fmul 	f5, 2.0
		sub		r3, 1
		cmp		r3, 0
		jg		loop1
wait:	jmp 	main

;		0x00 Jmp 18
;func:	0x03 Mov r2, 10
;loop:	0x07 Sub r2, 1
;		0x0b Cmp r2, 0
;		0x0f Jg -10
;		0x11 Ret
;main:	0x12 Movs r1, 16
;		0x16 Add r1, 8192
;		0x1a Fload f0, [r2]
;		0x1c Fadd f0, 3.5e+010
;		0x26 Fmov f1, 1e+010
;		0x30 Fsub f0, f1
;		0x32 Fstore 16[r1], f0
;		0x36 Call 3
;		0x39 Mov r3, 5
;		0x3d Fmov f2, 0.5
;loop1:	0x47 Fmul f5, 2
;		0x51 Sub r3, 1
;		0x55 Cmp r3, 0
;		0x59 Jg -20
;wait:	0x5b Jmp 91