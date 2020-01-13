#include <check.h>

#include "../include/project/operations.h"
#include "../include/project/vm_lc3_types.h"

LC3State set_up_state(void) {
    LC3State state = {
        .running = 0, .error = 0, .error_msg = "", .reg = {0}, .memory = {0}};
    state.reg[R_PC] = PC_START;
    return state;
}

START_TEST(test_instr_add_register_mode) {
    LC3State state = set_up_state();
    size_reg instr_add = ((OP_ADD & 0x0F) << 12) | ((R_R0 & 0x07) << 9) |
                         ((R_R1 & 0x07) << 6) | ((R_R2 & 0x07));

    state.memory[PC_START] = instr_add;
    state.reg[R_R1] = 4;
    state.reg[R_R2] = 5;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 9);
    ck_assert_int_eq(state.reg[R_COND], FL_POS);
}
END_TEST

START_TEST(test_instr_add_immediate_mode) {
    LC3State state = set_up_state();
    size_reg instr_add = ((OP_ADD & 0x0F) << 12) | ((R_R0 & 0x07) << 9) |
                         ((R_R1 & 0x07) << 6) | (1 << 5) | 0x05;

    state.memory[PC_START] = instr_add;
    state.reg[R_R1] = 4;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 9);
    ck_assert_int_eq(state.reg[R_COND], FL_POS);
}
END_TEST

START_TEST(test_instr_and_register_mode) {
    LC3State state = set_up_state();
    size_reg instr_and = ((OP_AND & 0x0F) << 12) | ((R_R0 & 0x07) << 9) |
                         ((R_R1 & 0x07) << 6) | ((R_R2 & 0x07));
    state.memory[PC_START] = instr_and;
    state.reg[R_R1] = 0xFF;
    state.reg[R_R2] = 0xA0;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 0xA0);
    ck_assert_int_eq(state.reg[R_COND], FL_POS);
}
END_TEST

START_TEST(test_instr_and_immediate_mode) {
    LC3State state = set_up_state();
    size_reg instr_and = ((OP_AND & 0x0F) << 12) | ((R_R0 & 0x07) << 9) |
                         ((R_R1 & 0x07) << 6) | (1 << 5) | 0x0F;

    state.memory[PC_START] = instr_and;
    state.reg[R_R1] = 0x0A;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 0x0A);
    ck_assert_int_eq(state.reg[R_COND], FL_POS);
}
END_TEST

START_TEST(test_instr_not) {
    LC3State state = set_up_state();
    size_reg instr_not = ((OP_NOT & 0x0F) << 12) | ((R_R0 & 0x07) << 9) |
                         ((R_R1 & 0x07) << 6) | 0x3F;

    state.memory[PC_START] = instr_not;
    state.reg[R_R1] = 0x7FFF;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 0x8000);
    ck_assert_int_eq(state.reg[R_COND], FL_NEG);
}
END_TEST

START_TEST(test_instr_branch_no_action) {
    LC3State state = set_up_state();
    size_reg instr_br = ((OP_BR & 0x0F) << 12) | (1 << 11) | 0x123;

    state.memory[PC_START] = instr_br;
    state.reg[R_COND] = 0;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_PC], 0x3001);
}
END_TEST

START_TEST(test_instr_branch_neg_flg) {
    LC3State state = set_up_state();
    size_reg instr_br = ((OP_BR & 0x0F) << 12) | (1 << 11) | 0x0FF;

    state.memory[PC_START] = instr_br;
    state.reg[R_COND] = FL_NEG;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_PC], 0x3100);
}
END_TEST

START_TEST(test_instr_branch_zro_flg) {
    LC3State state = set_up_state();
    size_reg instr_br = ((OP_BR & 0x0F) << 12) | (1 << 9) | 0x0FF;

    state.memory[PC_START] = instr_br;
    state.reg[R_COND] = FL_POS;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_PC], 0x3100);
}
END_TEST

START_TEST(test_instr_branch_pos_flg) {
    LC3State state = set_up_state();
    size_reg instr_br = ((OP_BR & 0x0F) << 12) | (1 << 10) | 0x0FF;

    state.memory[PC_START] = instr_br;
    state.reg[R_COND] = FL_ZRO;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_PC], 0x3100);
}
END_TEST

START_TEST(test_instr_jmp_reg) {
    LC3State state = set_up_state();
    size_reg instr_jmp = ((OP_JMP & 0x0F) << 12) | ((R_R0 & 0x07) << 6);

    state.memory[PC_START] = instr_jmp;
    state.reg[R_R0] = 0xF0F0;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_PC], 0xF0F0);
}
END_TEST

START_TEST(test_instr_jsr_pc_offset) {
    LC3State state = set_up_state();
    size_reg instr_jsr = ((OP_JSR & 0x0F) << 12) | (1 << 11) | (0x0FF);

    state.memory[PC_START] = instr_jsr;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R7], 0x3001);
    ck_assert_int_eq(state.reg[R_PC], 0x3100);
}
END_TEST

START_TEST(test_instr_jsr_reg_offset) {
    LC3State state = set_up_state();
    size_reg instr_jsr = ((OP_JSR & 0x0F) << 12) | ((R_R0 & 0x07) << 6);

    state.memory[PC_START] = instr_jsr;
    state.reg[R_R0] = 0xF0F0;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R7], 0x3001);
    ck_assert_int_eq(state.reg[R_PC], 0xF0F0);
}
END_TEST

START_TEST(test_instr_ld) {
    LC3State state = set_up_state();
    size_reg instr_ld = ((OP_LD & 0x0F) << 12) | ((R_R0 & 0x07) << 6) | 0xFF;

    state.memory[PC_START] = instr_ld;
    state.memory[0x3100] = 0xF0F0;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 0xF0F0);
    ck_assert_int_eq(state.reg[R_COND], FL_NEG);
}
END_TEST

START_TEST(test_instr_ldi) {
    LC3State state = set_up_state();
    size_reg instr_ldi = ((OP_LDI & 0x0F) << 12) | ((R_R0 & 0x07) << 6) | 0xFF;

    state.memory[PC_START] = instr_ldi;
    state.memory[0x3100] = 0x3200;
    state.memory[0x3200] = 0x00;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 0x00);
    ck_assert_int_eq(state.reg[R_COND], FL_ZRO);
}
END_TEST

START_TEST(test_instr_ldr) {
    LC3State state = set_up_state();
    size_reg instr_ldr = ((OP_LDR & 0x0F) << 12) | ((R_R0 & 0x07) << 9) |
                         ((R_R1 & 0x07) << 6) | 0x0F;

    state.memory[PC_START] = instr_ldr;
    state.reg[R_R1] = 0x5FF1;
    state.memory[0x6000] = 0x0F0F;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 0x0F0F);
    ck_assert_int_eq(state.reg[R_COND], FL_POS);
}
END_TEST

START_TEST(test_instr_lea) {
    LC3State state = set_up_state();
    size_reg instr_lea = ((OP_LEA & 0x0F) << 12) | ((R_R0 & 0x07) << 9) | 0xFF;

    state.memory[PC_START] = instr_lea;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.reg[R_R0], 0x3100);
    ck_assert_int_eq(state.reg[R_COND], FL_POS);
}
END_TEST

START_TEST(test_instr_st) {
    LC3State state = set_up_state();
    size_reg instr_st = ((OP_ST & 0x0F) << 12) | ((R_R0 & 0x07) << 9) | 0xFF;

    state.memory[PC_START] = instr_st;
    state.reg[R_R0] = 0xF0F0;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.memory[0x3100], 0xF0F0);
}
END_TEST

START_TEST(test_instr_sti) {
    LC3State state = set_up_state();
    size_reg instr_sti = ((OP_STI & 0x0F) << 12) | ((R_R0 & 0x07) << 9) | 0xFF;

    state.memory[PC_START] = instr_sti;
    state.reg[R_R0] = 0x0F0F;
    state.memory[0x3100] = 0x3200;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.memory[0x3200], 0x0F0F);
}
END_TEST

START_TEST(test_instr_str) {
    LC3State state = set_up_state();
    size_reg instr_str = ((OP_STR & 0x0F) << 12) | ((R_R0 & 0x07) << 9) |
                         ((R_R1 & 0x07) << 6) | 0x0F;

    state.memory[PC_START] = instr_str;
    state.reg[R_R0] = 0x0F0F;
    state.reg[R_R1] = 0x5FF1;

    read_execute(&state);

    ck_assert_int_eq(state.error, 0);
    ck_assert_str_eq(state.error_msg, "");
    ck_assert_int_eq(state.memory[0x6000], 0x0F0F);
}
END_TEST

int main(void) {
    Suite* s1 = suite_create("Core");
    SRunner* sr = srunner_create(s1);

    TCase* tc_operations = tcase_create("Operations");
    suite_add_tcase(s1, tc_operations);

    tcase_add_test(tc_operations, test_instr_add_register_mode);
    tcase_add_test(tc_operations, test_instr_add_immediate_mode);
    tcase_add_test(tc_operations, test_instr_and_register_mode);
    tcase_add_test(tc_operations, test_instr_and_immediate_mode);
    tcase_add_test(tc_operations, test_instr_not);
    tcase_add_test(tc_operations, test_instr_branch_no_action);
    tcase_add_test(tc_operations, test_instr_branch_neg_flg);
    tcase_add_test(tc_operations, test_instr_branch_zro_flg);
    tcase_add_test(tc_operations, test_instr_branch_pos_flg);
    tcase_add_test(tc_operations, test_instr_jmp_reg);
    tcase_add_test(tc_operations, test_instr_jsr_pc_offset);
    tcase_add_test(tc_operations, test_instr_jsr_reg_offset);
    tcase_add_test(tc_operations, test_instr_ld);
    tcase_add_test(tc_operations, test_instr_ldi);
    tcase_add_test(tc_operations, test_instr_ldr);
    tcase_add_test(tc_operations, test_instr_lea);
    tcase_add_test(tc_operations, test_instr_st);
    tcase_add_test(tc_operations, test_instr_sti);
    tcase_add_test(tc_operations, test_instr_str);

    srunner_run_all(sr, CK_VERBOSE);
    int nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}
