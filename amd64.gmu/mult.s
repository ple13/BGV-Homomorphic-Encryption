/* The MIT License (MIT)
 *
 * Copyright (c) 2015 mehdi sotoodeh
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

.include "defines.inc"

/* _______________________________________________________________________
/* MULSET_S0(YY,BB,XX)
/* Out:  CARRY:Y = b*X
/* _______________________________________________________________________ */

.macro MULSET_S0 YY,BB,XX
    MULT    \XX,\BB
    mov     ACL,\YY
.endm

/* _______________________________________________________________________
/* MULSET_S1(YY,BB,XX)
/* Out: CARRY:Y = b*X + CARRY
/* _______________________________________________________________________ */

.macro MULSET_S1 YY,BB,XX
    mov     ACH,C0
    MULT    \XX,\BB
    add     C0,ACL
    adc     $0,ACH
    mov     ACL,\YY
.endm

/* _______________________________________________________________________
/* MULADD_S0(YY,BB,XX)
/* Out:  CARYY:Y = b*X + Y
/* _______________________________________________________________________ */

.macro MULADD_S0 YY,BB,XX
    MULT    \XX,\BB
    add     ACL,\YY
    adc     $0,ACH
.endm

/* _______________________________________________________________________
/* MULADD_S1(YY,BB,XX)
/* Out: CARRY:Y = b*X + Y + CARRY
/*      ZF = set if no carry
/* _______________________________________________________________________ */

.macro MULADD_S1 YY,BB,XX
    mov     ACH,C0
    MULT    \XX,\BB
    add     C0,ACL
    adc     $0,ACH
    add     ACL,\YY
    adc     $0,ACH
.endm

.macro mulset3 AA,BB
    MULSET_S0 \AA,\BB,A0
    MULSET_S1 8+\AA,\BB,A1
    MULSET_S1 16+\AA,\BB,A2
    mov     ACH,24+\AA
.endm

.macro muladd3 AA,BB
    MULADD_S0 \AA,\BB,A0
    MULADD_S1 8+\AA,\BB,A1
    MULADD_S1 16+\AA,\BB,A2
    mov     ACH,24+\AA
.endm

/* _______________________________________________________________________
/*
/*   void ecp_MulReduce(U64* Z, const U64* X, const U64* Y)
/* Uses: A, B, C0
/* Constant-time
/* _______________________________________________________________________ */
    PUBPROC ecp_MulReduce

.equ  Z,  ARG1
.equ  X,  ARG2
.equ  Y,  ARG3

    PushB
    push    Z
    sub     $48,%rsp                /* T(6) */

.equ  T,  %rsp

    LOADA   Y
    LOADB   X

    mulset3 0(T), B0
    muladd3 8(T), B1
    muladd3 16(T),B2

    /* Now do the size reduction: T(3) + 933887*U(3) */

    MULADD_W0 A0,(T),24(T),$933887
    MULADD_W1 A1,8(T),32(T),$933887
    MULADD_W1 A2,16(T),40(T),$933887

    MULT    $933887,ACH
    ADDA    $0,ACH,ACL

    sbb     ACL,ACL
    and     $933887,ACL
    ADDA    $0,$0,ACL

    add     $48,%rsp
    pop     Z
    /* return result */
    STOREA  Z

    PopB
    ret

/* _______________________________________________________________________
/*
/*   void ecp_Mul(U64* Z, const U64* X, const U64* Y)
/* _______________________________________________________________________ */
    PUBPROC ecp_Mul

.equ  Z,  ARG1M
.equ  X,  ARG2
.equ  Y,  ARG3

    PushB
    SaveArg1

    LOADA   Y
    LOADB   X

    mulset3 0(Z), B0
    muladd3 8(Z), B1
    muladd3 16(Z),B2

    RestoreArg1
    PopB
    ret


