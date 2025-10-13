#include <assert.h>

#include "cpu_header.h"
#include "stages.h"

#ifndef ARR_WORK_HEADER
#define ARR_WORK_HEADER

void init_block_arr(BlockArr* block_arr);
CpuInfo find_block(Register curr_pc, BlockArr* block_arr, BaseBlock** ans);
BaseBlock* start_block(Register curr_pc, BlockArr* block_arr);
void end_block(Register curr_pc, BlockArr* block_arr);
void add_instr_to_block(BaseBlock* block, DecodedResult* new_instr);
void free_block_arr(BlockArr* block_arr);

#endif
