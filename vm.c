#include "vm.h"

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

void op_ldi(uint16_t instr){
  uint16_t r0 = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  reg[r0];
}
