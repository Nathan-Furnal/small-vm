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

/*
 * Extends the sign of an unsigned integer. Checks if a bit has value 1 after
 * the shift by `bit_count`. If it does, it is considered as having a leading 1,
 * thus being negative. In that case, the maximum value (0xFFFF) is left shifted
 * to meet at the `bit_count` cutoff and the variable is updated with a bitwise
 * `OR` mask.
 *
 * @param { uint16_t } x : the value to be extended.
 * @param { int } bit_count : the bit from which the shift happens.
 * @return { uint16_t } the value after extension.
 */
inline uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) { // checks if the first bit of the right bit
                                    // shifted value is 1
    x |= (0xFFFF << bit_count); // updates value with left shift of max value
  }
  return x;
}

/*
 * Updates `R_COND` with the proper flag for a given register.
 *
 * @param { uint16_t } r : the index of a register.
 */
inline void update_flags(uint16_t r) {
  if (reg[r] == 0) {
    reg[R_COND] = FL_ZRO;
  }
  // a 1 in the left-most bit indicates a negative value
  else if (reg[r] >> 15) {
    reg[R_COND] = FL_NEG;
  } else {
    reg[R_COND] = FL_POS;
  }
}
#endif
