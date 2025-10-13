#include "../include/instructions.h"
#include "../include/stages.h"

#define EXEC_ERROR   printf("error in execution\n" \
                            " :file: %s\n" \
                            "  func: %s\n" \
                            "  line: %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);

#define EXEC_ASSERT(cond, text)     if (!(cond)) \
                                    { \
                                        EXEC_ERROR \
                                        printf("details: bad %s\n\n", text); \
                                        exit(1);\
                                    }

void do_kAdd(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t rt)
{
    Register* regs = cpu_state->gpr_regs;
    regs[rd] = regs[rs] + regs[rt];

    // printf("%d %d %d\n", rd, rs, rt);
    // printf("ADD: %d\n", regs[rd]);
}


void do_kSub(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t rt)
{
    Register* regs = cpu_state->gpr_regs;
    regs[rd] = regs[rs] - regs[rt];
}


void do_kOr(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t rt)
{
    Register* regs = cpu_state->gpr_regs;
    regs[rd] = regs[rs] | regs[rt];
}


void do_kBext(CpuState* cpu_state, uint8_t rd, uint8_t rs1, uint8_t rs2)
{
    Register* regs = cpu_state->gpr_regs;
    
    uint32_t mask = regs[rs1] & regs[rs2]; //select bits
    uint32_t invert_mask = regs[rs2] ^ (-1); 

    regs[rd] &= invert_mask; //zero certain bits
    regs[rd] |= mask; //copy certan bits from rs1
}


/*
* syscalls:
* 0 = read, 1 = write, else = unknown 
*
* x8 = number of syscall, x0 = argument and result
*/
void do_kSyscall(CpuState* cpu_state)
{
    Register* regs = cpu_state->gpr_regs;

    switch(cpu_state->gpr_regs[8])
    {
        case 0:
        {
            scanf("%d", &regs[0]);
            break;
        }
        case 1:
        {
            printf("%d\n", regs[0]);
            break;
        }
        default:
        {   
            printf("Unknown syscall number\n");
            exit(0);
        }
    }
}


void do_kClz(CpuState* cpu_state, uint8_t rd, uint8_t rs)
{
    Register* regs = cpu_state->gpr_regs;

    Register count = 0; 
    uint32_t to_measure = regs[rs];
    size_t reg_size = sizeof(Register) * 8;
    for (size_t i = 0; i < reg_size; i++)
    {
        if (!((to_measure << i) >> (reg_size - 1))) {
            count++;
        }
        else 
            break;
    }

    regs[rd] = count;
}


void do_kSlti(CpuState* cpu_state, uint8_t rs, uint8_t rt, int16_t imm)
{
    Register* regs = cpu_state->gpr_regs;

    regs[rt] = (Register)(regs[rs] < (Register)imm);
}


void do_kSt(CpuState* cpu_state, uint8_t base, uint8_t rt, int16_t offset)
{
    EXEC_ASSERT((offset & 3) == 0, "align") //check allignment

    Register* regs = cpu_state->gpr_regs;

    // printf("val: %d, addr: %d\n", regs[rt], regs[base] + (Register)offset);

    write_to_mem(cpu_state, regs[base] + (Register)offset, regs[rt]);
}


void do_kSsat(CpuState* cpu_state, uint8_t rd, uint8_t rs, int16_t imm5)
{
    Register* regs = cpu_state->gpr_regs;
    Register max_val = (Register)pow(2, imm5 - 1) - 1, 
             min_val = (Register)(-pow(2, imm5 - 1)); 

    // printf("SSAT MAX VAL: %d, MIN: %d\n", max_val, min_val);

    if (regs[rs] > max_val)
    {
        regs[rd] = max_val;
    }
    else if (regs[rs] < min_val)
    {
        regs[rd] = min_val;
    }
    else 
    {
        regs[rd] = regs[rs];
    }
}


//offset = 10-bit number
void do_kLdp(CpuState* cpu_state, uint8_t base, uint8_t rt1, uint8_t rt2, int16_t offset)
{
    EXEC_ASSERT((offset & 3) == 0, "align") //check allignment

    Register* regs = cpu_state->gpr_regs;
    Register addr = regs[base] + (Register)offset;

    regs[rt1] = read_from_mem(cpu_state, addr);
    regs[rt2] = read_from_mem(cpu_state, addr + 4);
}


void do_kBeq(CpuState* cpu_state, uint8_t rs, uint8_t rt, int16_t offset)
{
    Register* regs = cpu_state->gpr_regs;
    Register target = ((Register)(offset) << 2);

    if (regs[rs] == regs[rt])
    {
        // printf("=== JUMP ===\n");
        cpu_state->pc += target;
    }
}


void do_kLd(CpuState* cpu_state, uint8_t base, uint8_t rt, int16_t offset)
{
    EXEC_ASSERT((offset & 3) == 0, "align") //check allignment

    Register* regs = cpu_state->gpr_regs;

    regs[rt] = read_from_mem(cpu_state, regs[base] + (Register)offset);
}


void do_kJ(CpuState* cpu_state, int32_t index)
{
    // printf("JUMP TO POS: %d\n", index);
    cpu_state->pc = ((Register)index << 2) | ((cpu_state->pc >> 28) << 28);
}


void do_kUsat(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t imm5)
{
    Register* regs = cpu_state->gpr_regs;
    Register max_val = (Register)pow(2, imm5) - 1; 
    // printf("USAT MAX VAL: %d\n", max_val);

    if (regs[rs] > max_val)
    {
        regs[rd] = max_val;
    }
    else 
    {
        regs[rd] = regs[rs];
    }
}
