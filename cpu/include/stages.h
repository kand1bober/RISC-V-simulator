
#include "dsl.h"

#ifndef STAGES_HEADER
#define STAGES_HEADER

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

typedef enum
{   
    //R-type (reg-reg)
    kRtype = 0,
    kAdd = 24,
    kOr = 16,
    kBext = 15,
    kSub = 25,
    kSyscall = 21,
    kClz = 28,

    //Other
    kSlti = 61,
    kSt = 56,
    kSsat = 63,    
    kLdp = 53,
    kBeq = 22,
    kLd = 62,
    kJ = 48,
    kUsat = 34,
} Opcode;
    

typedef enum
{
    kArg5,
    kArg11,
    kArg16,
    kArg25,  
    kArg25, 
} Operand;


typedef struct
{
    Opcode opcode;
    int num_operands;
    struct 
    {
        Operand type;
        union 
        {
            int32_t i32;
            int16_t i16;
            uint8_t i8;
        } val;

    } operands[MAX_OPERANDS];
} DecodedResult;


typedef struct 
{
    Opcode opcode;
    int num_operands;

    struct 
    {
        Operand type;
        int get_arg_func_num;
    } operands[MAX_OPERANDS];


} InstructionInfo;

static const InstructionInfo instructions_info[] = 
{
    #define SET_INSTR(opcode, num_op, ...) {opcode, num_op, {__VA_ARGS__}}, 

    SET_INSTR(kAdd,     3, {kArg5, 3}, {kArg5, 1}, {kArg5, 2})
    SET_INSTR(kSub,     3, {kArg5, 3}, {kArg5, 1}, {kArg5, 2})
    SET_INSTR(kOr,      3, {kArg5, 3}, {kArg5, 1}, {kArg5, 2})
    SET_INSTR(kBext,    3, {kArg5, 3}, {kArg5, 1}, {kArg5, 2})

    SET_INSTR(kSyscall, 1, {kArg, 3}, {kArg5, 1}, {kArg5, 2})

    #undef SET_INSTR
};

void fetch(CpuState* cpu_state, BufInfo* input, uint32_t* curr_cmd);
void decode(CpuState* cpu_state, uint32_t curr_cmd, DecodedResult* result);
void execute(CpuState* cpu_state, DecodedResult* instruction);
void write_to_mem(CpuState* cpu_state, Register addr, Register val);
Register read_from_mem(CpuState* cpu_state, Register addr);

#endif
