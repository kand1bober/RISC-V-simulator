#include "../include/include.h"

static const char* kCmdBinFilename = "common/cmd_bin.txt";

int main()
{
    //read instructions
    BufInfo code;
    extern const char* kCmdBinFilename;
    read_file(kCmdBinFilename, &code);

    //start interpreter
    CpuState cpu_state = {.cpu_status = kGood, 
                          .block_status = kNotBuilding, 
                          .pc = 0, 
                          .memory.capacity = 0};
    
    for (int i = 0; i < kNumRegs; i++) 
        cpu_state.gpr_regs[i] = 0;

    //====== TEST ======
    cpu_state.gpr_regs[2] = 7;
    cpu_state.gpr_regs[3] = 8;
    //==================

    uint32_t curr_cmd = 0;
    DecodedResult decoded;
    BlockArr block_arr;
    init_block_arr(&block_arr);
    BaseBlock* curr_block = NULL; //currently builded block

    while (1)
    {
        fetch(&cpu_state, &code, &curr_cmd);

        if (cpu_state.cpu_status == kInputEnd)
            break;


        decode(&cpu_state, curr_cmd, &decoded);
        if (decoded.opcode == kJ || decoded.opcode == kBeq)
        {
            printf("Jump Operation\n");
            switch (cpu_state.block_status)
            {
                case kBuilding: //fall through
                {
                    printf("    End building base block\n");
                    //end building
                    end_block(cpu_state.pc, &block_arr);
                    cpu_state.block_status = kNotBuilding;
                }
                case kNotBuilding:
                {
                    printf("    Executing operation\n");
                    //decode and execute 1 instr
                    execute(&cpu_state, &decoded, 1);
                    break;
                }
                default: 
                {
                    printf("Error in building base block");
                    exit(1);
                }
            }
        }
        else  
        {
            printf("Normal Operation\n");
            switch (cpu_state.block_status)
            {
                case kBuilding:
                {
                    printf("    adding instr to existing base block\n");
                    // decode, execute and add to curr block 1 instr
                    add_instr_to_block(curr_block, &decoded);
                    execute(&cpu_state, &decoded, 1);
                    break;
                }   
                case kNotBuilding:
                {
                    BaseBlock* ready_block;
                    CpuInfo status = find_block(cpu_state.pc, &block_arr, &ready_block);

                    switch (status)
                    {
                        case kBaseBlockFound:
                        {
                            printf("    Executing base block\n");
                            //execute ready block of N decoded instructions
                            execute(&cpu_state, ready_block->instr_arr, ready_block->sz);
                            break;
                        }
                        case kBaseBlockNotFound:
                        {
                            printf("    start building base block\n");
                            //start building block, decode and execute 1 instr., add to block 
                            curr_block = start_block(cpu_state.pc, &block_arr);                            
                            cpu_state.block_status = kBuilding;
                            add_instr_to_block(curr_block, &decoded);

                            execute(&cpu_state, &decoded, 1);
                            break;
                        }
                        default:
                        {
                            printf("Error in finding of base block");
                            exit(1);
                        }
                    }
                    break;
                }
                default: 
                {
                    printf("Error in building base block");
                    exit(1);
                }
            }
        }
    }

    free(code.buf);
    free(cpu_state.memory.data);
    free_block_arr(&block_arr);

    return 0;
}
