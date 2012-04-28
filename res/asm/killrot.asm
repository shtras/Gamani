		jmp		main

clamp:	fcmp	f9, 180 	; f9 - in/out param [-180, 180)
		jl		cl1
		fsub	f9, 360
cl1:	fcmp	f9, -180
		jge		cl2
		fadd	f9, 360
cl2:	ret

atan:	push	r0
		push	r1
		push	r2
		mov		r0, 0
		mov		r1, 0
		mov		r2, 0
		fcmp	f9, 0
		jl		al0
		jmp		cnt1
al0:	fneg	f9, f9
		mov		r0, 1
cnt1:	fcmp	f9, 1
		jg		ag1
		jmp		cnt2
ag1:	fmov	f10, 1
		fdiv	f10, f9
		fmov	f9, f10
		mov		r1, 1
cnt2:	fcmp	f9, 0.26794919243
		jg		agpi12
		jmp		cnt3
agpi12:	fmov	f10, f9
		fmul	f10, 0.577350269189625
		fadd	f10, 1
		fsub	f9, 0.577350269189625
		fdiv	f9, f10
		mov		r2, 1
cnt3:	fmov	f10, f9
		fmul	f10, f9
		fmul	f10, 0.4378497304
		fadd	f10, 1.6867629106
		fmov	f11, f9
		fmul	f11, f9
		fadd	f11, 1.6867633134
		fdiv	f10, f11
		fmul	f9, f10
		cmp		r2, 1
		je		region
		jmp		cnt4
region:	fadd	f9, 0.5235987755982
cnt4:	cmp		r1, 1
		je		compl
		jmp		cnt5
compl:	fmov	f10, 1.57079632679
		fsub	f10, f9
		fmov	f9, f10
cnt5:	cmp		r0, 1
		je 		sign
		jmp		cnt6
sign:	fneg 	f9, f9
cnt6:	pop		r2
		pop		r1
		pop		r0
		ret
	

;f9 - y, f10 - x
atan2:	fcmp	f10, 0
		jg		atanxg0
		je		atanxe0
		;x < 0
		fcmp	f9, 0
		jge		atanxl0yge0
		;x < 0, y < 0
		fdiv	f9, f10
		call	atan
		fsub	f9, 3.14159265
		ret
atanxl0yge0:	fdiv	f9, f10
				call	atan
				fadd	f9, 3.14159265
				ret
atanxg0:		fdiv	f9, f10
				call 	atan
				ret
atanxe0:		fcmp	f9, 0
				je		atanxe0ye0
				jg		atanxe0yg0
				;x = 0, y < 0
				fmov	f9, -1.57079633
				ret
atanxe0yg0:		fmov	f9, 1.57079633
				ret
atanxe0ye0:		fmov	f9, 0
				ret
		
main:	mov 	r1, 0x9000
		mov		[r1], rotate
		;movs	r1, 0x1
		;fmov	f7, -1
		;fstore	[r1], f7
		jmp 	wait
		
rtinit: mov		r0, 0x9508
		fload	f9, [r0]
		call 	clamp
		fmov	f0, f9		; start yaw
		fadd	f9, 270
		call 	clamp
		fmov	f1, f9 		; tgt yaw
		mov 	r1, 0x9000
		mov		[r1], rotate
		reti
		
rotate:	mov		r0, 0x9510
		fload	f5, [r0] ;vel[0]
		fload	f6, 0x8[r0] ;vel[1]
		fload	f7, 0x30[r0] ;refVel[0]
		fload	f8, 0x38[r0] ;refVel[1]
		fmov	f9, f5
		fsub	f9, f7
		fmov	f10, f8
		fsub	f10, f6
		call	atan2
		fmul	f9, 180
		fdiv	f9, 3.14159265
		call	clamp
		fmov	f1, f9		
		mov		r0, 0x9500
		fload	f2, [r0] 	; yaw vel
		mov		r0, 0x9508
		fload	f9, [r0]
		call 	clamp
		fmov	f3, f9	 	; yaw
		fmov	f9, f1
		fsub	f9, f3
		call 	clamp
		fmov	f4, f9		; curr delta (tgt yaw - yaw)
		fmul f2, 100
		fsub	f4, f2		; delta - yaw vel
		movs	r0, 0x1
		fmul	f4, 0.01
		fstore	[r0], f4
		reti
		
		
wait:	jmp 	wait		; busy wait

		;		interrupt vector:
		;		sensor update: 0x9000
		;		data vector:
		;		0x9500: 64 bit yaw velocity
		;		0x9508:	64 bit yaw
		;		0x9510: vel[0]
		;		0x9518: vel[1]
		;		0x9520: vel[2]
		;		0x9528: coord[0]
		;		0x9530: coord[1]
		;		0x9538: coord[2]
		;		0x9540: refVel[0]
		;		0x9548: refVel[1]
		;		0x9550: refVel[2]
		;		0x9558: refCoord[0]
		;		0x9560: refCoord[1]
		;		0x9568: refCoord[2]
		;		output vector:
		;		0x10000: 64bit yaw power [0, 1]
