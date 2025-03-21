#include <asm.h>

ENTRY(gettime)
    pushl %ebp
    movl %esp, %ebp

    pushl %ecx
    pushl %edx

    movl  $10, %eax

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


ENTRY(write)
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

ENTRY(getpid)
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

ENTRY(fork)
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