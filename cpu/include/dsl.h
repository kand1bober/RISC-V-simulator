#include "instructions.h"

#ifndef DSL_HEADER
#define DSL_HEADER


#define DECODE(instr_type, cmd) \
{ \
    InstructionInfo proto; \
    for (int i = 0; i < sizeof(instructions_info) / sizeof(InstructionInfo); i++) \
    { \
        if (instructions_info[i].opcode == instr_type) \
        { \
            proto = instructions_info[i]; \
        } \
        else \
            continue; \
    } \
    \
    result->num_operands = proto.num_operands; \
    for (int i = 0; i < proto.num_operands; i++) \
    { \
        Operand op_type = proto.operands[i].type; \
        result->operands[i].type = op_type; \
        switch (op_type) \
        { \
            case(kArg5): \
            { \
                switch (proto.operands[i].get_arg_func_num) \
                { \
                    case 1: \
                    { \
                        result->operands[i].val.i8 = GET_ARG_1(cmd); \
                        break; \
                    } \
                    case 2: \
                    { \
                        result->operands[i].val.i8 = GET_ARG_2(cmd); \
                        break; \
                    } \
                    case 3: \
                    { \
                        result->operands[i].val.i8 = GET_ARG_3(cmd); \
                        break; \
                    } \
                    default: \
                    { \
                       break; \
                    } \
                } \
            }\
            case (kArg11): \
            case (kArg16): \
            { \
                break; \
            } \
            case (kArg25): \
            { \
                break; \
            } \
            default: \
            { \
                printf("incorrect type\n"); \
                exit(0);\
            } \
        } \
    } \
} \


void decode_instr(Opcode instr_type, uint32_t cmd, DecodedResult* result)
{ 
    InstructionInfo proto; 
    for (int i = 0; i < sizeof(instructions_info) / sizeof(InstructionInfo); i++) 
    { 
        if (instructions_info[i].opcode == instr_type) 
        { 
            proto = instructions_info[i]; 
        } 
        else 
            continue; 
    } 
    
    result->num_operands = proto.num_operands; 
    for (int i = 0; i < proto.num_operands; i++) 
    { 
        Operand op_type = proto.operands[i].type; 
        result->operands[i].type = op_type; 
        switch (op_type) 
        { 
            case(kArg5): 
            { 
                switch (proto.operands[i].get_arg_func_num) 
                { 
                    case 1:
                    { 
                        result->operands[i].val.i8 = GET_ARG_1(cmd); 
                        break; 
                    } 
                    case 2: 
                    { 
                        result->operands[i].val.i8 = GET_ARG_2(cmd); 
                        break; 
                    } 
                    case 3: 
                    { 
                        result->operands[i].val.i8 = GET_ARG_3(cmd); 
                        break; 
                    } 
                    default: 
                    { 
                       break; 
                    } 
                } 
            }
            case (kArg11):
            case (kArg16): 
            { 
                break; 
            } 
            case (kArg25): 
            { 
                break; 
            } 
            default: 
            { 
                printf("incorrect type\n"); 
                exit(0);
            } 
        } 
    } 
} 


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