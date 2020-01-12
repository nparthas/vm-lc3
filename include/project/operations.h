#pragma once

#include <stdint.h>
#include <stdio.h>

#include "routines.h"
#include "vm_lc3_types.h"

FileReadStatus read_image_file(FILE* file, LC3State* state);

FileReadStatus read_image(const char* image_path, LC3State* state);

void mem_write(size_reg address, size_reg val, size_reg memory[]);

size_reg mem_read(size_reg address, size_reg memory[]);

void read_execute(LC3State* state);

/*** Assembly Operations **/

// TODO:: implement state
void branch(size_reg instr, LC3State* state);  // BR

void add(size_reg instr, LC3State* state);  // ADD

void load(size_reg instr, LC3State* state);  // LD

void store(size_reg instr, LC3State* state);  // ST

void jump_register(size_reg instr, LC3State* state);  // JSR

void and_bitwise(size_reg instr, LC3State* state);  // AND

void load_register(size_reg instr, LC3State* state);  // LDR

void store_register(size_reg instr, LC3State* state);  // STR

// NOT IMPLEMENTED
void return_from_interrupt(size_reg instr, LC3State* state);  // RTI

void not_bitwise(size_reg instr, LC3State* state);  // NOT

void load_indirect(size_reg instr, LC3State* state);  // LDI

void store_indirect(size_reg instr, LC3State* state);  // STI

void jump(size_reg instr, LC3State* state);  // JMP

// NOT IMPLEMENTED
void reserved(size_reg instr, LC3State* state);  // RES

void load_effective_address(size_reg instr, LC3State* state);  // LEA

void trap(size_reg instr, LC3State* state);  // TRAP
