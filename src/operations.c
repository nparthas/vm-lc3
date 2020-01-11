#include "project/operations.h"

uint16_t sign_extend(size_reg x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

void update_flags(Register reg_num, size_reg reg[]) {
    if (reg[reg_num] == 0) {
        reg[R_COND] = FL_ZRO;
    } else if (reg[reg_num] >> 15) {
        reg[R_COND] = FL_NEG;
    } else {
        reg[R_COND] = FL_POS;
    }
}

size_reg swap_endian(size_reg x) {
    return (x << (sizeof(x) * 4)) | (x >> (sizeof(x)) * 4);
}

FileReadStatus read_image_file(FILE* file, LC3State* state) {
    size_reg origin;
    fread(&origin, sizeof(origin), 1, file);

    if (ferror(file) || feof(file)) {
        return FR_FILE_READ_ERROR;
    }

    origin = swap_endian(origin);

    size_reg max_read = SIZE_REG_MAX - origin;
    size_reg* prog_ptr = state->memory + origin;
    size_t read = fread(prog_ptr, sizeof(size_reg), max_read, file);

    if (ferror(file)) {
        return FR_FILE_READ_ERROR;
    }

    while (read-- > 0) {
        *prog_ptr = swap_endian(*prog_ptr);
        ++prog_ptr;
    }
    return FR_NO_ERROR;
}

FileReadStatus read_image(const char* image_path, LC3State* state) {
    FILE* file = fopen(image_path, "rb");
    if (!file) {
        return FR_COULD_NOT_OPEN;
    }

    FileReadStatus status = read_image_file(file, state);
    if (status != FR_NO_ERROR) {
        return status;
    }

    if (fclose(file) == EOF) {
        return FR_COULD_NOT_CLOSE;
    }
    return FR_NO_ERROR;
}

void mem_write(size_reg address, size_reg val, size_reg memory[]) {
    memory[address] = val;
}

size_reg mem_read(size_reg address, size_reg memory[]) {
    if (address == MR_KBSR) {
        if (check_key()) {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        } else {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
}

void branch(size_reg instr, LC3State* state) {
    size_reg cond_flag = (instr >> 9) & 0x07;

    if (cond_flag & state->reg[R_COND]) {
        size_reg pc_offset = sign_extend(instr & 0x01FF, 9);
        state->reg[R_PC] += pc_offset;
    }
}

void add(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    Register r1 = (instr >> 6) & 0x07;

    size_reg imm_flag = (instr >> 5) & 0x01;  // check

    if (imm_flag) {
        size_reg imm = sign_extend(instr & 0x1F, 5);
        state->reg[r0] = state->reg[r1] + imm;
    } else {
        Register r2 = instr & 0x07;
        state->reg[r0] = state->reg[r1] + state->reg[r2];
    }

    update_flags(r0, state->reg);
}

void load(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    size_reg pc_offset = sign_extend(instr & 0x01FF, 9);

    state->reg[r0] = mem_read(state->reg[R_PC] + pc_offset, state->memory);
    update_flags(r0, state->reg);
}

void store(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    size_reg pc_offset = sign_extend(instr & 0x01FF, 9);
    mem_write(state->reg[R_PC] + pc_offset, state->reg[r0], state->memory);
}

void jump_register(size_reg instr, LC3State* state) {
    Register r1 = (instr >> 6) & 0x07;
    size_reg pc_offset = sign_extend(instr & 0x07FF, 11);
    size_reg flag = (instr >> 11) & 0x01;

    state->reg[R_R7] = state->reg[R_PC];
    if (flag) {
        state->reg[R_PC] += pc_offset;
    } else {
        state->reg[R_PC] = state->reg[r1];
    }
    // break here
}

void and_bitwise(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    Register r1 = (instr >> 6) & 0x07;

    size_reg imm_flag = (instr >> 5) & 0x01;  // check

    if (imm_flag) {
        size_reg imm = sign_extend(instr >> 0x1F, 5);
        state->reg[r0] = state->reg[r1] & imm;
    } else {
        Register r2 = instr & 0x07;
        state->reg[r0] = state->reg[r1] & state->reg[r2];
    }

    update_flags(r0, state->reg);
}

void load_register(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    Register r1 = (instr >> 6) & 0x07;
    size_reg offset = sign_extend(instr & 0x3F, 6);

    state->reg[r0] = mem_read(state->reg[r1] + offset, state->memory);
    update_flags(r0, state->reg);
}

void store_register(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    Register r1 = (instr >> 6) & 0x07;
    size_reg offset = sign_extend(instr & 0x3F, 6);
    mem_write(state->reg[r1] + offset, state->reg[r0], state->memory);
}

void return_from_interrupt(__unused size_reg instr, LC3State* state) {
    LC3_SET_ERROR(state, "Error: RTI is not implemented");
}

void not_bitwise(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    Register r1 = (instr >> 6) & 0x07;

    state->reg[r0] = ~state->reg[r1];
    update_flags(r0, state->reg);
}

void load_indirect(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    size_reg pc_offset = sign_extend(instr & 0x01FF, 9);

    state->reg[r0] = mem_read(
        mem_read(state->reg[R_PC] + pc_offset, state->memory), state->memory);
    update_flags(r0, state->reg);
}

void store_indirect(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    size_reg pc_offset = sign_extend(instr & 0x01FF, 9);
    mem_write(mem_read(state->reg[R_PC] + pc_offset, state->memory),
              state->reg[r0], state->memory);
}

void jump(size_reg instr, LC3State* state) {
    Register r1 = (instr >> 6) & 0x07;
    state->reg[R_PC] = state->reg[r1];
}

void reserved(__unused size_reg instr, LC3State* state) {
    LC3_SET_ERROR(state, "Error: RES is not implemented");
}

void load_effective_address(size_reg instr, LC3State* state) {
    Register r0 = (instr >> 9) & 0x07;
    size_reg pc_offset = sign_extend(instr & 0x01FF, 9);
    state->reg[r0] = state->reg[R_PC] + pc_offset;
    update_flags(r0, state->reg);
}

void trap(size_reg instr, LC3State* state) {
    switch (instr & 0xFF) {
        case TRAP_GETC:
            trap_getc(state);
            break;
        case TRAP_OUT:
            trap_out(state);
            break;
        case TRAP_PUTS:
            trap_puts(state);
            break;
        case TRAP_IN:
            trap_in(state);
            break;
        case TRAP_PUTSP:
            trap_putsp(state);
            break;
        case TRAP_HALT:
            trap_halt(state);
            break;
        default:
            LC3_SET_ERROR(state, "No matching trap found");
            break;
    }
}
