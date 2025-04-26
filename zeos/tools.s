# 0 "tools.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "tools.S"
# 1 "include/asm.h" 1
# 2 "tools.S" 2

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
    pushl %ebp
    movl %esp, %ebp

    pushl %esi
    pushl %edi
    pushl %ebx

    pushl 8(%ebp)

    call inner_task_switch

    addl $4, %esp

    popl %ebx
    popl %edi
    popl %esi

    movl %ebp, %esp
    popl %ebp
    ret

.globl get_ebp; .type get_ebp, @function; .align 0; get_ebp:
    movl %ebp, %eax
    ret

.globl stack_change; .type stack_change, @function; .align 0; stack_change:
    movl 4(%esp), %esp
    popl %ebp
    ret
