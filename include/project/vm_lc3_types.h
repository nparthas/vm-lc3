#pragma once

#include <stdint.h>
#include <string.h>

typedef uint16_t size_reg;

#define SIZE_REG_MAX UINT16_MAX
#define VM_LC3_ERROR_STR_LEN 50

#define LC3_SET_ERROR(state, error_str)                                 \
    do {                                                                \
        (state)->error = 1;                                             \
        strncpy((state)->error_msg, (error_str), VM_LC3_ERROR_STR_LEN); \
    } while (0)

typedef enum PcStart_t {
    PC_START = 0x3000,
} PcStart;

typedef enum Register_t {
    R_R0 = 0,  // 8 general purpose registers
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,            // program counter
    R_COND,          // condition register
    R_NUM_REGISTERS  // number of registers
} Register;

typedef enum MMapRegister_t {
    MR_KBSR = 0xFE00,  // keyboard status
    MR_KBDR = 0xFE02   // keyboard data
} MMapRegister;

typedef enum Operation_t {
    OP_BR = 0,  // branch
    OP_ADD,     // add
    OP_LD,      // load
    OP_ST,      // store
    OP_JSR,     // jump register
    OP_AND,     // bitsise and (&)
    OP_LDR,     // load register
    OP_STR,     // store register
    OP_RTI,     // return from interrupt (unused)
    OP_NOT,     // bitwise not (~)
    OP_LDI,     // load indirect
    OP_STI,     // store indirect
    OP_JMP,     // jump
    OP_RES,     // reserved (unused)
    OP_LEA,     // load effective address
    OP_TRAP     // execute trap
} Operation;

typedef struct LC3State_t {
    uint8_t running;
    uint8_t error;
    char error_msg[VM_LC3_ERROR_STR_LEN];
    size_reg reg[R_NUM_REGISTERS];
    size_reg memory[SIZE_REG_MAX];
} LC3State;

typedef enum FlagCondition_t {
    FL_POS = 1 << 0,  // positive
    FL_ZRO = 1 << 1,  // zero
    FL_NEG = 1 << 2   // negative
} FlagCondition;

typedef enum Trap_t {
    TRAP_GETC = 0x20,   // get keyboard character, no echo
    TRAP_OUT = 0x21,    // output character
    TRAP_PUTS = 0x22,   // output word string
    TRAP_IN = 0x23,     // get keyboard character, with echo
    TRAP_PUTSP = 0x24,  // output byte string
    TRAP_HALT = 0x25    // halt program
} Trap;

typedef enum FileReadStatus_t {
    FR_NO_ERROR = 0,
    FR_COULD_NOT_OPEN,
    FR_FILE_READ_ERROR,
    FR_COULD_NOT_CLOSE
} FileReadStatus;
