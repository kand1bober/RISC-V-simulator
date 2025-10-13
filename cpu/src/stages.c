#include "../include/include.h"

void fetch(CpuState* cpu_state, BufInfo* code, uint32_t* curr_cmd)
{
    if ((cpu_state->pc + CMD_SIZE - 1) >= code->sz) 
    {
        cpu_state->status = kInputEnd;
        return;
    }

    memcpy(curr_cmd, code->buf + cpu_state->pc, CMD_SIZE);
    cpu_state->pc += CMD_SIZE;

    cpu_state->status = kGood; 
}


void decode_instr(Opcode instr_type, uint32_t cmd, DecodedResult* result)
{ 
    InstructionInfo proto; 
    for (int i = 0; i < sizeof(instructions_info) / sizeof(InstructionInfo); i++) 
    { 
        if (instructions_info[i].opcode == instr_type) 
        {   
            proto = instructions_info[i]; 
            break;
        } 
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

                break; 
            }
            case (kArg11):
            {
                result->operands[i].val.i16 = GET_LAST_11(cmd); 
                break; 
            }
            case (kArg16): 
            { 
                result->operands[i].val.i16 = GET_LAST_16(cmd); 
                break; 
            } 
            case (kArg20): { break; } //syscall, no arguments
            case (kArg26): 
            { 
                result->operands[i].val.i32 = GET_LAST_26(cmd); 
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


void decode(CpuState* cpu_state, uint32_t curr_cmd, DecodedResult* result)
{
    DEB(CMD_DUMP(curr_cmd))
    DEB(CPU_DUMP(cpu_state))    
    DEB(MEM_DUMP)

    Opcode opcode = GET_TYPE(curr_cmd); 
    result->opcode = opcode;

    switch (opcode)
    {
        case kRtype:
        {
            Opcode R_type_opcode = GET_FUNC(curr_cmd); 
            result->opcode = R_type_opcode; 

            switch(R_type_opcode)
            {
                DECODE_CASE(kAdd, curr_cmd)
                DECODE_CASE(kSub, curr_cmd)
                DECODE_CASE(kOr, curr_cmd)
                DECODE_CASE(kBext, curr_cmd)
                DECODE_CASE(kSyscall, curr_cmd)
                DECODE_CASE(kClz, curr_cmd)
                
                default: {break;} 
            }

            break;
        }

        DECODE_CASE(kSlti, curr_cmd)
        DECODE_CASE(kSt, curr_cmd)
        DECODE_CASE(kSsat, curr_cmd)
        DECODE_CASE(kLdp, curr_cmd)
        DECODE_CASE(kBeq, curr_cmd)
        DECODE_CASE(kLd, curr_cmd)
        DECODE_CASE(kJ, curr_cmd)
        DECODE_CASE(kUsat, curr_cmd)

        default: {break;}
    }

}


#define OP(num, sz) instr->operands[num].val.i##sz
void execute(CpuState* cpu_state, DecodedResult* instr)
{
    switch (instr->opcode)
    {
        EXECUTE_CASE(kAdd, cpu_state, OP(0, 8), OP(1, 8), OP(2, 8))

        EXECUTE_CASE(kSub, cpu_state, OP(0, 8), OP(1, 8), OP(2, 8))

        EXECUTE_CASE(kOr, cpu_state, OP(0, 8), OP(1, 8), OP(2, 8))

        EXECUTE_CASE(kBext, cpu_state, OP(0, 8), OP(1, 8), OP(2, 8))

        EXECUTE_CASE(kSyscall, cpu_state)

        EXECUTE_CASE(kClz, cpu_state, OP(0, 8), OP(1, 8))

        EXECUTE_CASE(kSlti, cpu_state, OP(0, 8), OP(1, 8), OP(2, 16))

        EXECUTE_CASE(kSt, cpu_state, OP(0, 8), OP(1, 8), OP(2, 16))

        EXECUTE_CASE(kSsat, cpu_state, OP(0, 8), OP(1, 8), OP(0, 8))

        EXECUTE_CASE(kLdp, cpu_state, OP(0, 8), OP(1, 8), OP(2, 8), OP(3, 16))

        EXECUTE_CASE(kBeq, cpu_state, OP(0, 8), OP(1, 8), OP(2, 16))

        EXECUTE_CASE(kLd, cpu_state, OP(0, 8), OP(1, 8), OP(2, 16))

        EXECUTE_CASE(kJ, cpu_state, OP(0, 32))

        EXECUTE_CASE(kUsat, cpu_state, OP(0, 8), OP(1, 8), OP(2, 8))

        default: {break;}
    }

    DEB(CPU_DUMP(cpu_state))    
    DEB(MEM_DUMP)
}
#undef OP


void write_to_mem(CpuState* cpu_state, Register addr, Register val)
{
    if (addr < 0)
    {
        printf("Error: Wrong addr to write to memory\n");
        exit(0);
    }

    if (cpu_state->memory.capacity < addr + sizeof(Register))
    {   
        cpu_state->memory.capacity = (addr + sizeof(Register));
        cpu_state->memory.data = (char*)realloc(cpu_state->memory.data, cpu_state->memory.capacity);
        if (cpu_state->memory.data == NULL)
        {
            perror("realloc");
            exit(1);
        }
    }

    *(Register*)(cpu_state->memory.data + addr) = val;
}   


Register read_from_mem(CpuState* cpu_state, Register addr)
{
    if (addr < 0)
    {
        printf("Error: Wrong addr to read from\n");
        exit(0);
    }

    if (cpu_state->memory.capacity < addr + sizeof(Register))
    {   
        printf("Error: Trying to address unallocated memory region\n");
        exit(0);
    }

    return *(Register*)(cpu_state->memory.data + addr);
}