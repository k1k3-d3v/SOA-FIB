#include <asm.h>

ENTRY(addASM)
	push %ebp
	mov %esp,%edx
	mov 0x8(%ebp),%edx
	mov 0xc(%ebp),%eax
	add %edx,%eax
	pop %ebp
	ret
