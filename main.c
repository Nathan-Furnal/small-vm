#include "vm.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  /* Load arguments */
  if (argc < 2) {
    /* show usage string */
    printf("lc3 [image-file1] ...\n");
    exit(2);
  }

  for (int j = 1; j < argc; ++j) {
    if (!read_image(argv[j])) {
      printf("failed to load image: %s\n", argv[j]);
      exit(1);
    }
  }

  /* Setup, see utils.c */
  signal(SIGINT, handle_interrupt);
  disable_input_buffering();
  /* since exactly one condition flag should be set at any given time, set the Z
     flag */
  reg[R_COND] = FL_ZRO;
  reg[R_PC] = PC_START;
  int running = 1;
  while (running) {
    /* FETCH */
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >> 12;

    /* OPS */
    switch (op) {
    case OP_ADD:
      op_add(instr);
      break;
    case OP_AND:
      op_and(instr);
      break;
    case OP_NOT:
      op_not(instr);
      break;
    case OP_BR:
      op_br(instr);
      break;
    case OP_JMP:
      op_jmp(instr);
      break;
    case OP_JSR:
      op_jsr(instr);
      break;
    case OP_LD:
      op_ld(instr);
      break;
    case OP_LDI:
      op_ldi(instr);
      break;
    case OP_LDR:
      op_ldr(instr);
      break;
    case OP_LEA:
      op_lea(instr);
      break;
    case OP_ST:
      op_st(instr);
      break;
    case OP_STI:
      op_sti(instr);
      break;
    case OP_STR:
      op_str(instr);
      break;
    case OP_TRAP:
      switch_trap(instr);
      break;
    case OP_RES:
    case OP_RTI:
    default:
      puts("ABORTING...");
      abort();
      break;
    }
  }
  /* See utils.c */
  restore_input_buffering();
  return 0;
}
