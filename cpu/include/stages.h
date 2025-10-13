
#include "dsl.h"

#ifndef STAGES_HEADER
#define STAGES_HEADER

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
    kArg20, //special for syscall  
    kArg26, 
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
    SET_INSTR(kClz,     2, {kArg5, 1}, {kArg5, 2})
    SET_INSTR(kSyscall, 0, {kArg20})
    SET_INSTR(kSlti,    3, {kArg5, 1}, {kArg5, 2}, {kArg16})
    SET_INSTR(kSt,      3, {kArg5, 1}, {kArg5, 2}, {kArg16})
    SET_INSTR(kSsat,    3, {kArg5, 1}, {kArg5, 2}, {kArg5, 3})
    SET_INSTR(kLdp,     4, {kArg5, 1}, {kArg5, 2}, {kArg5, 3}, {kArg11})
    SET_INSTR(kBeq,     3, {kArg5, 1}, {kArg5, 2}, {kArg16})
    SET_INSTR(kLd,      3, {kArg5, 1}, {kArg5, 2}, {kArg16})
    SET_INSTR(kJ,       1, {kArg26})
    SET_INSTR(kUsat,    3, {kArg5, 1}, {kArg5, 2}, {kArg5, 3})

    #undef SET_INSTR
};

void fetch(CpuState* cpu_state, BufInfo* input, uint32_t* curr_cmd);
void decode_instr(Opcode instr_type, uint32_t cmd, DecodedResult* result);
void decode(CpuState* cpu_state, uint32_t curr_cmd, DecodedResult* result);
void execute(CpuState* cpu_state, DecodedResult* instruction);
void write_to_mem(CpuState* cpu_state, Register addr, Register val);
Register read_from_mem(CpuState* cpu_state, Register addr);

#endif
