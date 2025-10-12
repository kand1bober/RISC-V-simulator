#include "../include/include.h"

static const char* kCmdBinFilename = "common/cmd_bin.txt";

int main()
{
    //read instructions
    BufInfo code;
    extern const char* kCmdBinFilename;
    read_file(kCmdBinFilename, &code);

    //start interpreter
    CpuState cpu_state = {.status = kGood, .pc = 0, .memory.capacity = 0};
    uint32_t curr_cmd = 0;
    for (int i = 0; i < kNumRegs; i++) 
        cpu_state.gpr_regs[i] = 0;

    //===== TEST ======
    cpu_state.gpr_regs[2] = 7;
    cpu_state.gpr_regs[3] = 8;
    //=================

    DecodedResult instr;
    while (1)
    {
        fetch(&cpu_state, &code, &curr_cmd);

        if (cpu_state.status == kInputEnd)
            break;

        decode(&cpu_state, curr_cmd, &instr);
        execute(&cpu_state, &instr);
    }

    free(code.buf);
    free(cpu_state.memory.data);

    return 0;
}
