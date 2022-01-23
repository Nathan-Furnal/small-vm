#ifndef VM_H
#define VM_H
#include <stdint.h>
/* 65536 memory locations */
extern uint16_t memory[UINT16_MAX];

/* Registers,
 * 8 general purpose (R0-R7)
 * 1 program counter (PC)
 * 1 condition flags (COND)
 */

typedef enum {
  R_R0 = 0,
  R_R1,
  R_R2,
  R_R3,
  R_R4,
  R_R5,
  R_R6,
  R_R7,
  R_PC,
  R_COND,
  R_COUNT
} registers;

/* Number of registers */
extern uint16_t reg[R_COUNT];

/* Define the 16 opcodes of the LC-3 */

typedef enum {
  OP_BR = 0, /* branch */
  OP_ADD,    /* add  */
  OP_LD,     /* load */
  OP_ST,     /* store */
  OP_JSR,    /* jump register */
  OP_AND,    /* bitwise and */
  OP_LDR,    /* load register */
  OP_STR,    /* store register */
  OP_RTI,    /* unused */
  OP_NOT,    /* bitwise not */
  OP_LDI,    /* load indirect */
  OP_STI,    /* store indirect */
  OP_JMP,    /* jump */
  OP_RES,    /* reserved (unused) */
  OP_LEA,    /* load effective address */
  OP_TRAP    /* execute trap */
} opcodes;

/* Define the condition flags for R_COND */

typedef enum {
  FL_POS = 1 << 0, /* P */
  FL_ZRO = 1 << 1, /* Z */
  FL_NEG = 1 << 2, /* N */
} c_flags;

/* Starting default position */

typedef enum { PC_START = 0x3000 } start_position;
#endif
