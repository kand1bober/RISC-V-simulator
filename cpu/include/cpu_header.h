#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../utilities/file_work.h"

#ifndef CPU_HEADER
#define CPU_HEADER


    // #define DEBUG
#ifdef DEBUG
    #define DEB(expr) expr
#else 
    #define DEB(expr) 
#endif


typedef enum 
{
    kGood,
    kInputEnd,
} CpuInfo;


typedef struct
{   
    char* data; 
    size_t capacity;
} Memory;


#define kNumRegs 32
#define CMD_SIZE 4 
typedef int32_t Register;


typedef struct
{   
    CpuInfo status;
    Register gpr_regs[kNumRegs];
    Register pc;
    Memory memory;
} CpuState;

#endif
