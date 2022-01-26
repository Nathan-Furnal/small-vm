#ifndef VM_H
#define VM_H
#include <stdint.h>
#include <stdio.h>

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

/*
 * Starting default position. Does not start at 0x0 because anything between 0x0
 * and 0x3000 are addresses left empty to leave space for trap routine code.
 */
typedef enum { PC_START = 0x3000 } start_position;

/* Memory mapped registers */
typedef enum {
  MR_KBSR = 0xFE00, /* keyboard status */
  MR_KBDR = 0xFE02  /* keyboard data */
} mmap_registers;

/*
 * Writes a value at a given memory address, overwrites any existing value.
 *
 * @param { uint16_t } address : the address to write at.
 */
void mem_write(uint16_t address, uint16_t val);

/*
 * Reads a value at a given memory address. Checks if a memory mapped register
 * is used and gets the data if it's the case. Returns the value at the given
 * memory address in any case.
 *
 * @param { uint16_t } address : the memory address the read from.
 * @return { uint16_t } the value at the given address.
 */
uint16_t mem_read(uint16_t address);

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
uint16_t sign_extend(uint16_t x, int bit_count);

/*
 * Updates `R_COND` with the proper flag for a given register.
 *
 * @param { uint16_t } r : the index of a register.
 */
void update_flags(uint16_t r);

/*
 * Implements the add operation. Adding can be done either between two registers
 * or between a register and an immediate value.
 *
 * @param { uint16_t } instr : the input instruction.
 */
void op_add(uint16_t instr);

/*
 * Loads a value from a location in memory, into a register. Can be seen as an
 * access to far data since it loads a place in memory that points to a
 * different place in memory.
 *
 * @param { uint16_t } instr : the input instruction.
 */
void op_ldi(uint16_t instr);

void op_and(uint16_t instr);

void op_not(uint16_t instr);

void op_br(uint16_t instr);

void op_jmp(uint16_t instr);

void op_jsr(uint16_t instr);

void op_ld(uint16_t instr);

void op_ldr(uint16_t instr);

void op_lea(uint16_t instr);

void op_st(uint16_t instr);

void op_sti(uint16_t instr);

void op_str(uint16_t instr);

void op_trap(uint16_t instr);

typedef enum {
  TRAP_GETC =
      0x20, /* get character from keyboard, not echoed onto the terminal */
  TRAP_OUT = 0x21,   /* output a character */
  TRAP_PUTS = 0x22,  /* output a word string */
  TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
  TRAP_PUTSP = 0x24, /* output a byte string */
  TRAP_HALT = 0x25   /* halt the program */
} trap_codes;

void trap_puts();

void trap_getc();

void trap_out();

void trap_in();

void trap_putsp();

void trap_halt();

/* Ties all trap calls together */
void switch_trap(uint16_t instr);

/* I/O utilities */

uint16_t swap16(uint16_t x);
void read_image_file(FILE *file);
int read_image(const char *image_path);

#endif
