#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "project/operations.h"
#include "project/vm_lc3_types.h"

static struct termios original_io;

void disable_input_buffering(void) {
    tcgetattr(stdin->_file, &original_io);

    struct termios new_io = original_io;
    new_io.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(stdin->_file, TCSANOW, &new_io);
}

void restore_input_buffering() {
    tcsetattr(stdin->_file, TCSANOW, &original_io);
}

void interrupt_handler(__unused int signal) {
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        printf("vm-lc3 [image-file1] ...\n");
        return 2;
    }

    signal(SIGINT, interrupt_handler);
    disable_input_buffering();

    LC3State state = {
        .running = 0, .error = 0, .error_msg = "", .reg = {0}, .memory = {0}};

    for (int i = 1; i < argc; i++) {
        FileReadStatus status = read_image(argv[i], &state);
        if (status != FR_NO_ERROR) {
            printf("failed to load image %s\n", argv[i]);
            return status;
        }
    }

    state.reg[R_PC] = PC_START;

    state.running = 1;
    while (state.running && !state.error) {
        size_reg instr = mem_read(state.reg[R_PC]++, state.memory);
        size_reg op = instr >> 12;

        switch (op) {
            case OP_BR:
                branch(instr, &state);
                break;
            case OP_ADD:
                add(instr, &state);
                break;
            case OP_LD:
                load(instr, &state);
                break;
            case OP_ST:
                store(instr, &state);
                break;
            case OP_JSR:
                jump_register(instr, &state);
                break;
            case OP_AND:
                and_bitwise(instr, &state);
                break;
            case OP_LDR:
                load_register(instr, &state);
                break;
            case OP_STR:
                store_register(instr, &state);
                break;
            case OP_RTI:
                return_from_interrupt(instr, &state);
                break;
            case OP_NOT:
                not_bitwise(instr, &state);
                break;
            case OP_LDI:
                load_indirect(instr, &state);
                break;
            case OP_STI:
                store_indirect(instr, &state);
                break;
            case OP_JMP:
                jump(instr, &state);
                break;
            case OP_RES:
                reserved(instr, &state);
                break;
            case OP_LEA:
                load_effective_address(instr, &state);
                break;
            case OP_TRAP:
                trap(instr, &state);
                break;
            default:
                LC3_SET_ERROR(&state, "Error: operation not recognized");
                break;
        }
    }

    // do shutdown

    if (state.error) {
        printf("%s\n", state.error_msg);
        return 100;
    }

    printf("Exiting execution\n");
    return 0;
}
