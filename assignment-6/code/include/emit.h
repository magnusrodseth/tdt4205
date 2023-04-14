#ifndef EMIT_H_
#define EMIT_H_

#define RAX "%rax"
#define RBX "%rbx"
#define RCX "%rcx"
#define RDX "%rdx"
#define RSP "%rsp"
#define RBP "%rbp"
#define RSI "%rsi"
#define RDI "%rdi"
#define R8 "%r8"
#define R9 "%r9"
#define R10 "%r10"
#define R11 "%r11"
#define R12 "%r12"
#define R13 "%r13"
#define R14 "%r14"
#define R15 "%r15"
#define RIP "%rip"

#define MEM(reg) "("reg")"
#define ARRAY_MEM(array,index,stride) "("array","index","stride")"

#define DIRECTIVE(fmt, ...) printf(fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define LABEL(name, ...) printf(name":\n" __VA_OPT__(,) __VA_ARGS__)
#define EMIT(fmt, ...) printf("\t" fmt "\n" __VA_OPT__(,) __VA_ARGS__)

#define MOVQ(src,dst)     EMIT("movq %s, %s", (src), (dst))
#define PUSHQ(src)        EMIT("pushq %s", (src))
#define POPQ(src)         EMIT("popq %s", (src))

#define ADDQ(src,dst)     EMIT("addq %s, %s", (src), (dst))
#define SUBQ(src,dst)     EMIT("subq %s, %s", (src), (dst))
#define NEGQ(reg)         EMIT("negq %s", (reg))

#define IMULQ(src,dst)    EMIT("imulq %s, %s", (src), (dst))
#define CQO               EMIT("cqo"); // Sign extend RAX -> RDX:RAX
#define IDIVQ(by)         EMIT("idivq %s", (by))

#define RET               EMIT("ret")

#define CMPQ(op1,op2)     EMIT("cmpq %s, %s", (op1), (op2))
#define JNE(label)        EMIT("jne %s", (label))


#endif // EMIT_H_
