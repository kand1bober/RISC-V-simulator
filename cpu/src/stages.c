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


// void decode_exec(CpuState* cpu_state, uint32_t curr_cmd)
// {
//     DEB(CMD_DUMP(curr_cmd))
//     DEB(CPU_DUMP(cpu_state))    
//     DEB(MEM_DUMP)

//     switch (GET_TYPE(curr_cmd))
//     {
//         case 0:
//         {
//             switch(GET_FUNC(curr_cmd))
//             {
//                 CMD_CASE(kAdd, cpu_state, GET_ARG_3(curr_cmd), GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd))

//                 CMD_CASE(kSub, cpu_state, GET_ARG_3(curr_cmd), GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd))

//                 CMD_CASE(kOr, cpu_state, GET_ARG_3(curr_cmd), GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd))

//                 CMD_CASE(kBext, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_ARG_3(curr_cmd))

//                 CMD_CASE(kSyscall, cpu_state)

//                 CMD_CASE(kClz, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd))

//                 default:{
//                     break;
//                 } 
//             }

//             break;
//         }

//         CMD_CASE(kSlti, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_LAST_16(curr_cmd))

//         CMD_CASE(kSt, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_LAST_16(curr_cmd))

//         CMD_CASE(kSsat, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_ARG_3(curr_cmd))

//         CMD_CASE(kLdp, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_ARG_3(curr_cmd), GET_LAST_11(curr_cmd))

//         CMD_CASE(kBeq, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_LAST_16(curr_cmd))

//         CMD_CASE(kLd, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_LAST_16(curr_cmd))

//         CMD_CASE(kJ, cpu_state, GET_LAST_26(curr_cmd))

//         CMD_CASE(kUsat, cpu_state, GET_ARG_1(curr_cmd), GET_ARG_2(curr_cmd), GET_ARG_3(curr_cmd))

//         default:{
//             break;
//         }
//     }

//     DEB(CMD_DUMP(curr_cmd))
//     DEB(CPU_DUMP(cpu_state))    
//     DEB(MEM_DUMP)
// }


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