#include <libc.h>

extern int addASM(int a, int b);

char buff[24];

int pid;

int add(int par1, int par2) {
	return par1 + par2;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
   int var = addASM(0x42, 0x666);
   (void)var; //Para que no salte el warning de UNUSED VARIABLE VAR
   gettime();

  while(1) { }
}
