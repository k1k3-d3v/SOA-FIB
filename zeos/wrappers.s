# 0 "wrappers.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2

.globl gettime; .type gettime, @function; .align 0; gettime:
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl $10, %eax

    pushl $return
    pushl %ebp
    movl %esp, %ebp

    sysenter

return:
    cmpl $0, %eax
    jl eaxNegative
    jmp final

eaxNegative:
    neg %eax
    movl %eax, errno
    movl $-1, %eax

final:
    popl %ebp
    popl %edx
    popl %ecx
    popl %ebp
    ret
