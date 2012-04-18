#pragma once

/*

opcode
oooo oo
0x00 mov
0x01 add
0x02 sub
0x03 and
0x04 or
0x05 not
0x06 xor
0x07 asl
0x08 asr
0x09 push
0x0a pop
0x0b pswl
0x0c psws
0x0d
0x0e
0x0f

for shifted second operand 's' is added to opcode (mov --> movs)

0x10 cmp a, b                ; PSW set as in sub a, b
0x11 jmp oooo ooamode
0x12 je  oooo oott tttt tttt ; Z == 1
0x13 jne oooo oott tttt tttt ; Z == 0
0x14 jg  oooo oott tttt tttt ; S == 0, Z == 0
0x15 jge oooo oott tttt tttt ; S == 0
0x16 call
0x17 ret

oooo oo
0x20: fmov ddd sss
0x21: fadd
0x22: fsub
0x23: fmul
0x24: fdiv
0x25: fload fff amode 0
0x26: fstore amode 0 fff
0x27: fcmp

operand

PSW:
ZCSO - zero carry sign overflow. Modified by each instruction that modifies dest register

registers
GPR
rrr
0x0 - 0x5: r0 - r5
0x6: rsp - stack pointer
0x7: rip - instruction pointer

FPR:
fff
0x0 - 0x6: f0 - f6
0x7: 64-bit immediate

GPR addressing modes:
oooo ooaa arrr faaa rrr0 ;f - flag for source operand: 0 - regular, 1 - shifted
for each amode except 101 and 111 the last bit is reserved
000: rrr - register
001: [rrr] - register
010: [rrr++]
011: [rrr--]
100: 
101: iiii
110: iiii iiii [rrr]
111: iiii iiii iiii iiii - immediate

memory (x4bit):
0x00 0000
...
0xe0 0000

        0x0000: movs r1, 0xe0   ; 0000 0000 0001 1111 0000 0000 1110 0000                                                         001F00E0
        0x0008: mov  r2, [r1]   ; 0000 0000 0010 0001 0010                                                                        00212
        0x000d: push r2         ; 0010 0100 0010                                                                                  0AD1
        0x0010: movs [r2], 0x1  ; 0000 0000 1010 1101 0001                                                                        40153
        0x0015: xor  r3, r3     ; 0001 1000 0011 0000 0110                                                                        18306
loop:   0x001a: add  r3, 1      ; 0000 0100 0011 0101 0001                                                                        04351
        0x001f: cmp  r3, 10     ; 0100 0000 0011 0101 1010                                                                        4035A
        0x0024: jne  loop       ; 0100 1111 1111 0010                                                                             4FF2
        0x0028: fmov f5, 1.3e4  ; 1000 0010 1111 0100 0000 1100 1001 0110 0100 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000  82F40C9640000000
        0x0041: mov 15[r2], r1  ; 0000 0010 1001 1110 0000 0010                                                                   29E02
*/

class ADP
{
public:
private:
};
