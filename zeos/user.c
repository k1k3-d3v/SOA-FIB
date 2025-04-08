#include <libc.h>

extern int addASM(int a, int b);

char buff[24];

int pid;
char array[100];

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
   

  /*TEST PAGE_FAULT 
  int *p = 0x0;
  *p = 0;
  */

  //TEST BLOCK/UNBLOCK
  int pid = fork();

  if (pid == 0) {
    // Child process
    write(1, "Child process: Blocking itself\n", 31);
    block(); // Child blocks itself
    write(1, "Child process: Unblocked by parent\n", 36);
  } else if (pid > 0) {
    // Parent process
    write(1, "Parent process: Waiting before unblocking child\n", 48);
    for (int i = 0; i < 100000000; i++) {
      asm volatile("nop");
    }
    write(1, "Parent process: Unblocking child\n", 34);
    unblock(pid); // Parent unblocks the child
  }
 
  while(1) {}
}
