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

.globl getpid; .type getpid, @function; .align 0; getpid:
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl $20, %eax

    pushl $return3
    pushl %ebp
    movl %esp, %ebp

    sysenter

return3:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jl eaxNegative3
    jmp final3

eaxNegative3:
    negl %eax
    movl %eax, errno
    movl $-1, %eax

final3:
    movl %ebp, %esp
    popl %ebp
    ret

.globl fork; .type fork, @function; .align 0; fork:
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl $2, %eax

    pushl $return4
    pushl %ebp
    movl %esp, %ebp

    sysenter

return4:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jl eaxNegative4
    jmp final4

eaxNegative4:
    negl %eax
    movl %eax, errno
    movl $-1, %eax

final4:
    movl %ebp, %esp
    popl %ebp
    ret

.globl exit; .type exit, @function; .align 0; exit:
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl $1, %eax

    pushl $return5
    pushl %ebp
    movl %esp, %ebp

    sysenter

return5:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jl eaxNegative5
    jmp final5

eaxNegative5:
    negl %eax
    movl %eax, errno
    movl $-1, %eax

final5:
    movl %ebp, %esp
    popl %ebp
    ret

.globl block; .type block, @function; .align 0; block:
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl $21, %eax

    pushl $return6
    pushl %ebp
    movl %esp, %ebp

    sysenter

return6:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jl eaxNegative6
    jmp final6

eaxNegative6:
    negl %eax
    movl %eax, errno
    movl $-1, %eax

final6:
    movl %ebp, %esp
    popl %ebp
    ret

.globl unblock; .type unblock, @function; .align 0; unblock:
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl 8(%ebp), %edx

    movl $22, %eax

    pushl $return7
    pushl %ebp
    movl %esp, %ebp

    sysenter

return7:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    cmpl $0, %eax
    jl eaxNegative7
    jmp final7

eaxNegative7:
    negl %eax
    movl %eax, errno
    movl $-1, %eax

final7:
    movl %ebp, %esp
    popl %ebp
    ret
