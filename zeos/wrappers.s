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
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jl eaxNegative
    jmp final

eaxNegative:
    negl %eax
    movl %eax, errno
    movl $-1, %eax

final:
    movl %ebp, %esp
    popl %ebp
    ret


.globl write; .type write, @function; .align 0; write:
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl $4, %eax
    movl 8(%ebp), %edx
    movl 12(%ebp), %ecx
    movl 16(%ebp), %ebx

    pushl $return2
    pushl %ebp
    movl %esp, %ebp

    sysenter

return2:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jl eaxNegative2
    jmp final2

eaxNegative2:
    negl %eax
    movl %eax, errno
    movl $-1, %eax

final2:
    movl %ebp, %esp
    popl %ebp
    ret
