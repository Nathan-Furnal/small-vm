#include "vm.h"
#include "utils.c" // reading keyboard input

void mem_write(uint16_t address, uint16_t val) { memory[address] = val; }

uint16_t mem_read(uint16_t address) {
  if (address == MR_KBSR) {
    if (check_key()) { // defined in utils.c
      memory[MR_KBSR] = (1 << 15);
      memory[MR_KBDR] = getchar();
    } else {
      memory[MR_KBSR] = 0;
    }
  }
  return memory[address];
}

uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) { // checks if the first bit of the right bit
                                    // shifted value is 1
    x |= (0xFFFF << bit_count); // updates value with left shift of max value
  }
  return x;
}

void update_flags(uint16_t r) {
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

void op_add(uint16_t instr) {
  /* destination register (DR) */
  uint16_t r0 = (instr >> 9) & 0x7; // boolean mask of size 3 (0b0111)
  /* first operand (SR1) */
  uint16_t r1 = (instr >> 6) & 0x7;
  /* whether we are in immediate mode */
  uint16_t imm_flag = (instr >> 5) & 0x1; // boolean mask of size 1 (0b0001)
  if (imm_flag) {
    uint16_t imm5 =
        sign_extend(instr & 0x1F, 5); // boolean mask of size 5 (0b0001 1111)
    reg[r0] = reg[r1] + imm5;
  } else {
    uint16_t r2 = instr & 0x7;
    reg[r0] = reg[r1] + reg[r2];
  }
  update_flags(r0);
}

void op_ldi(uint16_t instr) {
  uint16_t r0 = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  /* add pc_offset to the current PC, look at that memory location to get
     the final address */
  reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
  update_flags(r0);
}

void op_and(uint16_t instr) {
  uint16_t r0 = (instr >> 9) & 0x7;
  uint16_t r1 = (instr >> 6) & 0x7;
  uint16_t imm_flag = (instr >> 5) & 0x1;
  if (imm_flag) {
    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
    reg[r0] = reg[r1] & imm5;
  } else {
    uint16_t r2 = instr & 0x7;
    reg[r0] = reg[r1] & reg[r2];
  }
  update_flags(r0);
}

void op_not(uint16_t instr) {
  uint16_t r0 = (instr >> 9) & 0x7;
  uint16_t r1 = (instr >> 6) & 0x7;
  reg[r0] = ~reg[r1];
  update_flags(r0);
}

void op_br(uint16_t instr) {
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  uint16_t cond_flag = (instr >> 9) & 0x7;
  if (cond_flag & reg[R_COND]) {
    reg[R_PC] += pc_offset;
  }
}

void op_jmp(uint16_t instr) {
  /* Also handles RET */
  uint16_t r1 = (instr >> 6) & 0x7;
  reg[R_PC] = reg[r1];
}

void op_jsr(uint16_t instr) {
  uint16_t long_flag = (instr >> 11) & 1;
  reg[R_R7] = reg[R_PC];
  if (long_flag) {
    uint16_t long_pc_offset = sign_extend(instr & 0x7FF, 11);
    reg[R_PC] += long_pc_offset; /* JSR */
  } else {
    uint16_t r1 = (instr >> 6) & 0x7;
    reg[R_PC] = reg[r1]; /* JSRR */
  }
}

void op_ld(uint16_t instr) {
  uint16_t r0 = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  reg[r0] = mem_read(reg[R_PC] + pc_offset);
  update_flags(r0);
}

void op_ldr(uint16_t instr) {
  uint16_t r0 = (instr >> 9) & 0x7;
  uint16_t r1 = (instr >> 6) & 0x7;
  uint16_t r_offset = sign_extend(instr & 0x3F, 6);
  reg[r0] = mem_read(reg[r1] + r_offset);
  update_flags(r0);
}

void op_lea(uint16_t instr) {
  uint16_t r0 = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  reg[r0] = reg[R_PC] + pc_offset;
  update_flags(r0);
}

void op_st(uint16_t instr) {
  uint16_t r1 = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  mem_write(reg[R_PC] + pc_offset, reg[r1]);
}

void op_sti(uint16_t instr) {
  uint16_t r1 = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  mem_write(mem_read(reg[R_PC] + pc_offset), reg[r1]);
}

void op_str(uint16_t instr) {
  uint16_t r1 = (instr >> 9) & 0x7;
  uint16_t r2 = (instr >> 6) & 0x7;
  uint16_t offset = sign_extend(instr & 0x3F, 6);
  mem_write(reg[r2] + offset, reg[r1]);
}

void trap_puts() {
  /* one char per word */
  uint16_t *c = memory + reg[R_R0];
  while (*c) {
    putc((char)*c, stdout);
    ++c;
  }
  fflush(stdout);
}

void trap_getc() {
  /* read a single ASCII char */
  reg[R_R0] = (uint16_t)getchar();
  update_flags(R_R0);
}

void trap_out() {
  putc((char)reg[R_R0], stdout);
  fflush(stdout);
}

void trap_in() {
  printf("Enter a character: ");
  char c = getchar();
  putc(c, stdout);
  fflush(stdout);
  reg[R_R0] = (uint16_t)c;
  update_flags(R_R0);
}

void trap_putsp() {
  /* one char per byte (two bytes per word) here we need to swap back to big
     endian format */
  uint16_t *c = memory + reg[R_R0];
  while (*c) {
    char char1 = (*c) & 0xFF;
    putc(char1, stdout);
    char char2 = (*c) >> 8;
    if (char2)
      putc(char2, stdout);
    ++c;
  }
  fflush(stdout);
}

void trap_halt() {
  puts("HALT");
  fflush(stdout);
  exit(0);
}

void switch_trap(uint16_t instr) {
  uint16_t trap_instr = instr & 0xFF;
  switch (trap_instr) {
  case TRAP_GETC:
    trap_getc();
    break;
  case TRAP_OUT:
    trap_out();
    break;
  case TRAP_PUTS:
    trap_puts();
    break;
  case TRAP_IN:
    trap_in();
    break;
  case TRAP_PUTSP:
    trap_putsp();
    break;
  case TRAP_HALT:
    trap_halt();
    break;
  }
}

/// Utilities to read a LC-3 file and load it into memory ///

uint16_t swap16(uint16_t x) { return (x << 8) | (x >> 8); }

void read_image_file(FILE *file) {
  /* the origin tells us where in memory to place the image */
  uint16_t origin;
  fread(&origin, sizeof(origin), 1, file);
  origin = swap16(origin);

  /* we know the maximum file size so we only need one fread */
  uint16_t max_read = UINT16_MAX - origin;
  uint16_t *p = memory + origin;
  size_t read = fread(p, sizeof(uint16_t), max_read, file);

  /* swap to little endian */
  while (read-- > 0) {
    *p = swap16(*p);
    ++p;
  }
}

int read_image(const char *image_path) {
  FILE *file = fopen(image_path, "rb");
  if (!file) {
    return 0;
  };
  read_image_file(file);
  fclose(file);
  return 1;
}
