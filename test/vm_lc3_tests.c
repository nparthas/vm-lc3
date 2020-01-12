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

int main(void) {
    Suite* s1 = suite_create("Core");
    TCase* tc1_1 = tcase_create("Core");
    SRunner* sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, test_instr_add_register_mode);
    tcase_add_test(tc1_1, test_instr_add_immediate_mode);
    tcase_add_test(tc1_1, test_instr_and_register_mode);
    tcase_add_test(tc1_1, test_instr_and_immediate_mode);

    srunner_run_all(sr, CK_VERBOSE);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}
