#include <unistd.h>
#include <sys/syscall.h>
#include <math.h>

#include "cpu_header.h"

#ifndef INSTRUCTIONS_HEADER
#define INSTRUCTIONS_HEADER

void do_kAdd(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t rt);
void do_kSub(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t rt);
void do_kOr(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t rt);
void do_kBext(CpuState* cpu_state, uint8_t rd, uint8_t rs1, uint8_t rs2);
void do_kSyscall(CpuState* cpu_state);
void do_kClz(CpuState* cpu_state, uint8_t rd, uint8_t rs);
void do_kSlti(CpuState* cpu_state, uint8_t rs, uint8_t rt, int16_t imm);
void do_kSt(CpuState* cpu_state, uint8_t base, uint8_t rt, int16_t offset);
void do_kSsat(CpuState* cpu_state, uint8_t rd, uint8_t rs, int16_t imm5);
void do_kLdp(CpuState* cpu_state, uint8_t base, uint8_t rt1, uint8_t rt2, int16_t offset);
void do_kBeq(CpuState* cpu_state, uint8_t rs, uint8_t rt, int16_t offset);
void do_kLd(CpuState* cpu_state, uint8_t base, uint8_t rt, int16_t offset);
void do_kJ(CpuState* cpu_state, int32_t index);
void do_kUsat(CpuState* cpu_state, uint8_t rd, uint8_t rs, uint8_t imm5);

#endif 