#pragma once

#include <stdio.h>
#include <sys/select.h>
#include <time.h>

#include "vm_lc3_types.h"

void trap_getc(LC3State* state);

void trap_out(LC3State* state);

void trap_puts(LC3State* state);

void trap_in(LC3State* state);

void trap_putsp(LC3State* state);

void trap_halt(LC3State* state);

size_reg check_key(void);
