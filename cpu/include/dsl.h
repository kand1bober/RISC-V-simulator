#include "instructions.h"

#ifndef DSL_HEADER
#define DSL_HEADER

#define MAX_OPERANDS 4

#define TYPE_MASK 0b0111111 //6 bits == 1
#define TYPE_SHIFT 26 
#define GET_TYPE(cmd) (((cmd) >> TYPE_SHIFT) & TYPE_MASK) 

#define OPERAND_MASK 31 //5 bits == 1 
#define ARG_1_SHIFT 21 
#define ARG_2_SHIFT 16 
#define ARG_3_SHIFT 11
#define GET_ARG_1(cmd) (((cmd) >> ARG_1_SHIFT) & OPERAND_MASK)  
#define GET_ARG_2(cmd) (((cmd) >> ARG_2_SHIFT) & OPERAND_MASK)  
#define GET_ARG_3(cmd) (((cmd) >> ARG_3_SHIFT) & OPERAND_MASK)  

#define GET_LAST_11(cmd) ((cmd) & 2047) //11 bits == 1
#define GET_LAST_16(cmd) ((cmd) & 65535) //16 bits == 1  
#define GET_LAST_26(cmd) ((cmd) & 67108863) //26 bits == 1

//for R-type
#define FUNC_MASK 31 //5 bits == 1
#define GET_FUNC(cmd) ((cmd) & FUNC_MASK)


#define DECODE_CASE(instr_type, cmd) \
case(instr_type): \
{ \
    decode_instr(instr_type, cmd, result); \
    break; \
}\


#define EXECUTE_CASE(code, ...) \
case(code): \
{ \
    do_##code(__VA_ARGS__); \
    break; \
}\


#define CPU_DUMP(cpu_state) \
    printf("Registers:\n"\
           "pc: %-2d\n", cpu_state->pc);\
    for (int i = 0; i < kNumRegs; i += 4) \
    { \
        for (int j = 0; j < 4; j++) \
        { \
            printf("x%-2d: %-5d  ", i + j, cpu_state->gpr_regs[i + j]); \
        } \
        printf("\n"); \
    } \
    printf("\n");

#define CMD_DUMP(curr_cmd)\
    printf("=====cmd=====:\n"\
        "type: %d\n"\
        "func(for R-type): %d\n"\
        "arg1: %d\n"\
        "arg2: %d\n"\
        "arg3: %d\n"\
        "last10: %d\n"\
        "last15: %d\n"\
        "last25: %d\n"\
        "=============\n\n",\
        GET_TYPE(curr_cmd),\
        GET_FUNC(curr_cmd),\
        GET_ARG_1(curr_cmd),\
        GET_ARG_2(curr_cmd),\
        GET_ARG_3(curr_cmd),\
        GET_LAST_11(curr_cmd),\
        GET_LAST_16(curr_cmd),\
        GET_LAST_26(curr_cmd));

#define MEM_DUMP \
    { \
        printf("Memory:\n"); \
        size_t reg_size = sizeof(Register); \
        for (int i = 0; (i * reg_size) < cpu_state->memory.capacity; i++) \
        { \
            printf("%-3d: %-4d\n", i, *(Register*)(cpu_state->memory.data + i * reg_size)); \
            printf("\n"); \
        } \
        printf("\n"); \
    } 

#endif