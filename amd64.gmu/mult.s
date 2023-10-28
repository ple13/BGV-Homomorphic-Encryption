.include "defines.inc"

/* q = 2^192 - 933887 */
/* t = 2^64  - 114687 */

/* _______________________________________________________________________
/* CF:Y = b*X
/* _______________________________________________________________________ */

.macro MULSET_S0 YY,BB,XX
    MULT    \XX,\BB
    mov     ACL,\YY
.endm

/* _______________________________________________________________________
/* CF:Y = b*X + CF
/* _______________________________________________________________________ */

.macro MULSET_S1 YY,BB,XX
    mov     ACH,C0
    MULT    \XX,\BB
    add     C0,ACL
    adc     $0,ACH
    mov     ACL,\YY
.endm

/* _______________________________________________________________________
/* CARYY:Y = b*X + Y
/* _______________________________________________________________________ */

.macro MULADD_S0 YY,BB,XX
    MULT    \XX,\BB
    add     ACL,\YY
    adc     $0,ACH
.endm

/* _______________________________________________________________________
/* CF:Y = b*X + Y + CF
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

.macro mulset1 AA,BB
    MULSET_S0 \AA,\BB,A0
    mov     ACH,8+\AA
.endm

.macro muladd1 AA,BB
    MULADD_S0 \AA,\BB,A0
    mov     ACH,8+\AA
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
/* void asmMulModQ(uint64_t* Z, const uint64_t* X, const uint64_t* Y)
/* _______________________________________________________________________ */
    PUBPROC asmMulModQ

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

    /* CF(i):T(i) = T(i) + 933887*T(i+3) + CF(i-1)                   */

    MULADD_W0 A0,(T),24(T),$933887
    MULADD_W1 A1,8(T),32(T),$933887
    MULADD_W1 A2,16(T),40(T),$933887

    /* Currently holding temp = ACH*2^192 + A2*2^128 + A1*2^64 + A0        */
    MULT    $933887,ACH
    ADDA    $0,ACH,ACL      /* ACH:A2A1A0 = ACH*(2^128 - 933887) + A2A1A0  */

    sbb     ACL,ACL         /* ACL = -ACH */
    and     $933887,ACL     /* ACL = -933887*ACH */
    ADDA    $0,$0,ACL       /* A2A1A0 -= ACL */

    add     $48,%rsp
    pop     Z
    /* return result */
    STOREA  Z

    PopB
    ret

/* _______________________________________________________________________
/*
/*   void asmMulModQ(uint64_t* Z, const uint64_t* X)
/* _______________________________________________________________________ */
    PUBPROC asmMulWithPModQ

.equ  Z,  ARG1
.equ  X,  ARG2
.equ  Y,  ARG3

    push    Z
    sub     $32,%rsp                /* T(6) */

.equ  T,  %rsp

    LOADA   X
    mov     $18446744073709436929, A3

    mulset3 0(T), A3

    mov     (T), A0
    mov     8(T), A1
    mov     16(T), A2

    /* Currently holding temp = ACH*2^192 + A2*2^128 + A1*2^64 + A0        */
    MULT    $933887,ACH
    ADDA    $0,ACH,ACL      /* ACH:A2A1A0 = ACH*(2^128 - 933887) + A2A1A0  */

    sbb     ACL,ACL         /* ACL = -ACH */
    and     $933887,ACL     /* ACL = -933887*ACH */
    ADDA    $0,$0,ACL       /* A2A1A0 -= ACL */

    add     $32,%rsp
    pop     Z
    /* return result */
    STOREA  Z

    ret

/* _______________________________________________________________________
/*
/*   void asmMulModP(uint64_t* Z, const uint64_t* X, const uint64_t* Y)
/* Uses: A, B, C0
/* Constant-time
/* _______________________________________________________________________ */
    PUBPROC asmMulModP

.equ  Z,  ARG1
.equ  X,  ARG2
.equ  Y,  ARG3

    MULT    (X), (Y)        /* ACH:ACL = XY */          /*ACH < 2^64 - 2*114687 */
    mov     ACL, A0         /* A0 = ACL */
    MULT    $114687, ACH    /* ACH:ACL = 114687*ACH */  /*ACH < 11468 */
    mov     ACL, A1         /* A1 = ACL */
    MULT    $114687, ACH    /* ACH:ACL = 114687*ACH */  /*ACH = 0 */
    add     ACL, A1         /* A1 += ACL */
    adc     A1, A0          /* A0 += A1 + ACH */
    sbb     ACL, ACL
    and     $114687, ACL    /* ACL = 114687*CF */  /*ACH = 0 */
    add     ACL, A0         /* A0 += ACL */

    /* return result */
    STOREA0  Z

    ret


/* _______________________________________________________________________
/*
/*   Z = X + Y
/*   void asmAddModQ(uint64_t* Z, const uint64_t* X, const uint64_t* Y)
/* _______________________________________________________________________ */

    PUBPROC asmAddModQ

    LOADA   Y
    ADDA    16(X),8(X),(X)  /* A = X + Y - CF*2^192 */

    sbb     ACL,ACL         /* ACL = -CF */
    and     $933887,ACL     /* ACL = CF*933887 */
    ADDA    $0,$0,ACL       /* A = (X + Y) - CF*Q */

    STOREA  Z
    ret

/* _______________________________________________________________________
/*
/*   Z = X - Y % Q
/*   void asmSubModQ(uint64_t* Z, const uint64_t* X, const uint64_t* Y)
/* _______________________________________________________________________ */
    PUBPROC asmSubModQ

    SaveArg3
    LOADA   X
    SUBA    16(Y),8(Y),(Y)      /* X - Y, CF --> 2^192*CF + (X-Y)*/

    sbb     ACL,ACL             /* ACL = -CF */
    and     $933887,ACL         /* ACL = 933887*CF */
    SUBA    $0,$0,ACL           /* 2^192*CF + (X-Y) - 933887*CF = Q*CF + (X-Y) */

    STOREA  Z
    RestoreArg3
    ret

/* _______________________________________________________________________
/*
/*   Z = X + Y
/*   void asmAddModP(uint64_t* Z, const uint64_t* X, const uint64_t* Y)
/* _______________________________________________________________________ */

    PUBPROC asmAddModP

    mov     (Y), A0
    add     (X), A0             /* A = X + Y - CF*2^64 */

    sbb     ACL, ACL            /* ACL = -CF */
    and     $114687, ACL        /* ACL = CF*114687 */
    add     ACL, A0             /* A = (X + Y) - CF*P */

    STOREA0  Z
    ret

/* _______________________________________________________________________
/*
/*   Z = X - Y % P
/*   void asmSubModP(uint64_t* Z, const uint64_t* X, const uint64_t* Y)
/* _______________________________________________________________________ */
    PUBPROC asmSubModP

    mov     (X), A0
    sub     (Y), A0             /* A0 = X-Y + 2^64*CF */

    sbb     ACL, ACL            /* ACL = -CF */
    and     $114687, ACL        /* ACL = CF*114687 */
    sbb     ACL, A0             /* A0 = X-Y + CF*P */

    STOREA0  Z
    ret

/* _______________________________________________________________________
/*
/*   void asmModPInQ(uint64_t* Z, uint64_t* X)
/*   Constant-time
/* _______________________________________________________________________ */
    PUBPROC asmModPInQ
.equ  Z,  ARG1
.equ  X,  ARG2

    LOADA   X               /* A0 = X0 */
    MULT    $114687, A1     /* ACH:ACL = 114687*X1 */  /*ACH < 11468 */
    mov     ACL, A1         /* A1 = ACL */
    MULT    $114687, ACH    /* ACH:ACL = 114687*ACH */  /*ACH = 0 */
    add     ACL, A1         /* A1 += ACL */
    adc     A1, A0          /* A0 += A1 + ACH */
    sbb     ACL, ACL
    and     $114687, ACL    /* ACL = 114687*CF */  /*ACH = 0 */
    add     ACL, A0         /* A0 += ACL */

    MULT    $13153107969, A2/* ACH:ACL = 114687*X2 */  /*ACH < 13153107969 */
    mov     ACL, A1         /* A1 = ACL */
    MULT    $114687, ACH    /* ACH:ACL = 13153107969*ACH */  /*ACH = 0 */
    add     ACL, A1         /* A1 += ACL */
    adc     A1, A0          /* A0 += A1 + ACH */
    sbb     ACL, ACL
    and     $114687, ACL    /* ACL = 114687*CF */  /*ACH = 0 */
    add     ACL, A0         /* A0 += ACL */

    sbb     ACL, ACL
    and     $114687, ACL    /* ACL = 114687*CF */  /*ACH = 0 */
    add     ACL, A0         /* A0 += ACL */

    STOREA0 Z
    ret

/* _______________________________________________________________________
/*
/*   compare X and Q/2, return X-Q/2 if X > Q/2
/*   int asmCorrect(const uint64_t* X)
/* _______________________________________________________________________ */
    PUBPROC asmCorrect

.equ  X,  ARG1
.equ  Y,  ARG2
    PUSHB
    LOADA   X
    movq    $9223372036854775807, B2
    movq    $18446744073709551615, B1
    movq    $18446744073709084672, B0
    SUBB    A2, A1, A0
    sbb     ACL,ACL
    movq    $1508490492706816, B0
    and     B0, ACL
    SUBA    $0, $0, ACL
    POPB
    STOREA  X
    ret
