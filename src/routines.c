#include "project/routines.h"

void trap_getc(LC3State* state) { state->reg[R_R0] = (size_reg)getchar(); }

void trap_out(LC3State* state) {
    putc((char)state->reg[R_R0], stdout);
    fflush(stdout);
}

void trap_puts(LC3State* state) {
    size_reg* c = state->memory + state->reg[R_R0];

    while (*c) {
        putc((char)*c, stdout);
        ++c;
    }
    fflush(stdout);
}

void trap_in(LC3State* state) {
    printf("Enter a character: ");

    char c = getchar();
    putc(c, stdout);

    state->reg[R_R0] = (size_reg)c;
}

void trap_putsp(LC3State* state) {
    size_reg* c = state->memory + state->reg[R_R0];
    while (*c) {
        char c1 = (*c) & 0xFF;
        putc(c1, stdout);

        char c2 = (*c) >> 8;
        if (c2) {
            putc(c2, stdout);
        }

        ++c;
    }

    fflush(stdout);
}

void trap_halt(LC3State* state) {
    puts("HALTING");
    fflush(stdout);
    state->running = 0;
}

size_reg check_key(void) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(stdin->_file, &readfds);

    struct timeval timeout = {.tv_sec = 0, .tv_usec = 0};
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}
