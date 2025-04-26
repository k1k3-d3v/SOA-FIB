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
   //int var = addASM(0x42, 0x666);
   //(void)var; //Para que no salte el warning de UNUSED VARIABLE VAR
   

  /*TEST PAGE_FAULT 
  int *p = 0x0;
  *p = 0;
  */

  /*
  while (1) {
    for (int i = 0; i < 100000000; i++) {
      asm volatile("nop");
    }
    if (pid == 0) {
      write(1, "El proceso hijo ha hecho exit\n", 30);
      exit();
    }
  }
  */

  
  /*
  while(1) {
    if(getpid() >= 1000) {
      write(1, "Soy el hijo\n", 12);
      char pid_str[10];
      itoa(getpid(), pid_str);
      write(1, pid_str, strlen(pid_str));
      write(1, "\n", 1);
      write(1, "Exiting process\n", 17);
      exit();
      exit();
    }
  } 
  */

   //TEST FORK
   //pid = fork();

  /*TEST BLOCK/UNBLOCK
  if (pid > 0)  {
    write(1, "[PADRE] Desbloqueando hijo\n", 27);

		int block = unblock(pid);

		if (block < 0) {
      write(1,"FALLO_UNBLOCK\n",14);
    }
    else {
      write(1,"BIEN_UNBLOCK\n",13);
    }
	}
	else if (pid == 0){
    for (int i = 0; i < 10000000; i++) {
      asm volatile("nop");
    }
    write(1, "[HIJO] Me bloqueo\n", 18);
		block();
		write(1, "[HIJO] UNBLOCKED\n", 17);
	}
  */

  /*
  if (pid > 0)  {

    for (int i = 0; i < 10000000; i++) {
      asm volatile("nop");
    }

    write(1, "[PADRE] Desbloqueando hijo\n", 27);

		int block = unblock(pid);

		if (block < 0) {
      write(1,"FALLO_UNBLOCK\n",14);
    }
    else {
      write(1,"BIEN_UNBLOCK\n",13);
    }
	}
	else if (pid == 0){
    write(1, "[HIJO] Me bloqueo\n", 18);
		block();
		write(1, "[HIJO] UNBLOCKED\n", 17);
	}
  */
 
  while(1) {}
}
