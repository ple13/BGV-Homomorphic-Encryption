/*
    Definitions for GNU assembler
    ABI: gcc
*/

/* Restore these registers across calls: rsp, rbx, rbp, r12,r13,r14,r15 */

/* 128-bit accumulator */
.equ ACL,     %rax
.equ ACH,     %rdx
.equ CARRY,   ACH

/* 192-bit accumulator-A */
/* Accumulator-A uses volatile registers */
.equ A0,      %r8
.equ A1,      %r9
.equ A2,      %r10
.equ A3,      %r11

/* 192-bit accumulator-B */
.equ B0,      %r12
.equ B1,      %r13
.equ B2,      %r14

.equ C0,      %rcx
.equ C1,      %rbx
.equ C2,      %rdi
.equ C3,      %rsi

.ifdef MSVC
/* From left to right */
.equ ARG1,    %rcx
.equ ARG2,    %rdx
.equ ARG3,    %r8
.equ ARG4,    %r9

/* ARG switching */
.equ ARG1M,   %rdi
.equ ARG2M,   %rsi
.equ ARG3M,   %rbx
.equ ARG4M,   %rbp

.macro  SaveArg1
    push    ARG1M
    mov     ARG1,ARG1M
.endm

.macro  RestoreArg1
    pop     ARG1M
.endm

.macro  SaveArg2
    push    ARG2M
    mov     ARG2,ARG2M
.endm

.macro  RestoreArg2
    pop     ARG2M
.endm

.macro  SaveArg3
    push    ARG3M
    mov     ARG3,ARG3M
.endm

.macro  RestoreArg3
    pop     ARG3M
.endm

.macro  SaveArg4
    push    ARG4M
    mov     ARG4,ARG4M
.endm

.macro  RestoreArg4
    pop     ARG4M
.endm
.else
/* From left to right */
.equ ARG1,    %rdi
.equ ARG2,    %rsi
.equ ARG3,    %rdx
.equ ARG4,    %rcx
.equ ARG5,    %r8
.equ ARG6,    %r9

/* ARG switching */
.equ ARG1M,   %rdi
.equ ARG2M,   %rsi
.equ ARG3M,   %rbx
.equ ARG4M,   %rbp

.macro  SaveArg1
.endm

.macro  RestoreArg1
.endm

.macro  SaveArg2
.endm

.macro  RestoreArg2
.endm

.macro  SaveArg3
    push    ARG3M
    mov     ARG3,ARG3M
.endm

.macro  RestoreArg3
    pop     ARG3M
.endm

.macro  SaveArg4
    push    ARG4M
    mov     ARG4,ARG4M
.endm

.macro  RestoreArg4
    pop     ARG4M
.endm
.endif

.macro  PUBPROC pname
.text
.align 8
.globl \pname
\pname:
.endm

.macro  PushB
    push    B2
    push    B1
    push    B0
.endm

.macro  PopB
    pop     B0
    pop     B1
    pop     B2
.endm

.macro  STOREA addr
    mov     A2,16(\addr)
    mov     A1,8(\addr)
    mov     A0,(\addr)
.endm

.macro  LOADA addr
    mov     16(\addr),A2
    mov     8(\addr),A1
    mov     (\addr),A0
.endm

.macro  LOADB addr
    mov     16(\addr),B2
    mov     8(\addr),B1
    mov     (\addr),B0
.endm

.macro  STOREB addr
    mov     B2,16(\addr)
    mov     B1,8(\addr)
    mov     B0,(\addr)
.endm

.macro PushA0
    push    A0
.endm

.macro PopA0
    pop     A0
.endm

.macro LOADB0 addr
    mov     (\addr),B0
.endm

.macro PushB0
    push    B0
.endm

.macro PopB0
    pop     B0
.endm

.macro  STOREA0 addr
    mov     A0,(\addr)
.endm

.macro LOADA0 addr
    mov     (\addr),A0
.endm

.macro  STOREB0 addr
    mov     B0,(\addr)
.endm

/* _______________________________________________________________________
//Y += X
// _______________________________________________________________________ */
.macro  ADD3 y2,y1,y0, x2,x1,x0
    add     \x0, \y0
    adc     \x1, \y1
    adc     \x2, \y2
.endm

.macro  ADDA x2,x1,x0
    ADD3 A2,A1,A0, \x2,\x1,\x0
.endm

.macro  ADDB x2,x1,x0
    ADD3 B2,B1,B0, \x2,\x1,\x0
.endm

/* _______________________________________________________________________
Y -= X
// _______________________________________________________________________ */
.macro  SUB3 y2,y1,y0, x2,x1,x0
    sub     \x0, \y0
    sbb     \x1, \y1
    sbb     \x2, \y2
.endm

.macro  SUBA x2,x1,x0
    SUB3 A2,A1,A0, \x2,\x1,\x0
.endm

.macro  SUBB x2,x1,x0
    SUB3 B2,B1,B0, \x2,\x1,\x0
.endm

/* _______________________________________________________________________
// ACH:ACL = a*b
// _______________________________________________________________________ */
.macro  MULT XX,YY
    mov     \XX,ACL
    mulq    \YY
.endm

/* _______________________________________________________________________
// u:v = a*b
// _______________________________________________________________________ */
.macro  MULSET u,v,a,b
    MULT    \a,\b
    mov     ACL,\v
    mov     ACH,\u
.endm

/* _______________________________________________________________________
// u:v += a*b
// _______________________________________________________________________ */
.macro  MULADD u,v,a,b
    MULT    \a,\b
    add     ACL,\v
    adc     ACH,\u
.endm


/* _______________________________________________________________________
// CARRY:Z = b*X + Y
// _______________________________________________________________________ */

.macro  MULADD_W0 ZZ,YY,BB,XX
    MULT    \XX,\BB
    add     \YY,ACL
    adc     $0,ACH
    mov     ACL,\ZZ
.endm

/* _______________________________________________________________________
// CARRY:Z = b*X + Y + CARRY
//      ZF = set if no carry
// _______________________________________________________________________ */

.macro  MULADD_W1 ZZ,YY,BB,XX
    mov     ACH,\ZZ     /* Z = CARRY                                       */
    MULT    \XX,\BB     /* ACH:ACL = b*X                                   */
    add     \YY,ACL     /* ACL = ACL + Y                                   */
    adc     $0,ACH      /* ACH = ACH + CARRY                               */
    add     ACL,\ZZ     /* Z = Z + ACL                                     */
    adc     $0,ACH      /* ACH = ACH + CARRY                               */
.endm




