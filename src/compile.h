#pragma once

#include <linux/bpf.h>

#include "lang/ast.h"

#define INSN(_code, _dst, _src, _off, _imm)	\
	((struct bpf_insn) {			\
		.code  = _code,			\
		.dst_reg = _dst,		\
		.src_reg = _src,		\
		.off   = _off,			\
		.imm   = _imm			\
	})

#define MOV(_dst, _src)     INSN(BPF_ALU64 | BPF_MOV | BPF_X, _dst, _src, 0, 0)
#define MOV_IMM(_dst, _imm) INSN(BPF_ALU64 | BPF_MOV | BPF_K, _dst, 0, 0, _imm)

#define EXIT INSN(BPF_JMP | BPF_EXIT, 0, 0, 0, 0)
#define CALL(_imm) INSN(BPF_JMP | BPF_CALL, 0, 0, 0, _imm)
#define JMP(_op, _dst, _src, _off)     INSN(BPF_JMP | BPF_OP((_op)) | BPF_X, _dst, _src, _off, 0)
#define JMP_IMM(_op, _dst, _imm, _off) INSN(BPF_JMP | BPF_OP((_op)) | BPF_K, _dst, 0, _off, _imm)

#define ALU(_op, _dst, _src)     INSN(BPF_ALU64 | BPF_OP((_op)) | BPF_X, _dst, _src, 0, 0)
#define ALU_IMM(_op, _dst, _imm) INSN(BPF_ALU64 | BPF_OP((_op)) | BPF_K, _dst, 0, 0, _imm)

#define STW_IMM(_dst, _off, _imm) INSN(BPF_ST  | BPF_SIZE(BPF_W)  | BPF_MEM, _dst, 0, _off, _imm)
#define STXW(_dst, _off, _src)    INSN(BPF_STX | BPF_SIZE(BPF_W)  | BPF_MEM, _dst, _src, _off, 0)
#define STXDW(_dst, _off, _src)   INSN(BPF_STX | BPF_SIZE(BPF_DW) | BPF_MEM, _dst, _src, _off, 0)

#define LDXB(_dst, _off, _src)  INSN(BPF_LDX | BPF_SIZE(BPF_B)  | BPF_MEM, _dst, _src, _off, 0)
#define LDXW(_dst, _off, _src)  INSN(BPF_LDX | BPF_SIZE(BPF_W)  | BPF_MEM, _dst, _src, _off, 0)
#define LDXDW(_dst, _off, _src) INSN(BPF_LDX | BPF_SIZE(BPF_DW) | BPF_MEM, _dst, _src, _off, 0)

#define RET_ON_ERR(_err, _fmt, ...)					\
	if (_err) {							\
		fprintf(stderr, "error(%s:%d): " _fmt, __func__, _err,	\
			##__VA_ARGS__);					\
	}


struct ebpf {
	struct bpf_insn *ip;
	struct bpf_insn  prog[BPF_MAXINSNS];
};

void emit(struct ebpf *e, struct bpf_insn insn);

static inline void emit_ld_mapfd(struct ebpf *e, int reg, int fd)
{
	emit(e, INSN(BPF_LD | BPF_DW | BPF_IMM, reg, BPF_PSEUDO_MAP_FD, 0, fd));
	emit(e, INSN(0, 0, 0, 0, 0));
}

struct ebpf *compile_probe(node_t *probe);
