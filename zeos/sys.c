/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#include <list.h>
#define LECTURA 0
#define ESCRIPTURA 1
#define BLOCK 128

char buff[128];

extern int zeos_ticks;

extern struct list_head free_queue;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  if (list_first( &free_queue)  == NULL) {
    return -1;
  }

  copy_data(current()-);

  struct list_head * e1 = list_first(&free_queue);
  list_del(e1);
  copy_data(current()->)
      
  init_task = list_head_to_task_struct(e1);
  init_task_union = (union task_union*)init_task;
  
  init_task->PID = 1;
  allocate_DIR(init_task);
  
  set_user_pages(init_task);
  tss.esp0 = (unsigned long)&init_task_union->stack[KERNEL_STACK_SIZE];			//Pasar de puntero a entero
  writeMSR(0x175, (unsigned long)&init_task_union->stack[KERNEL_STACK_SIZE]); 	//Pasar de puntero a entero
  set_cr3(init_task->dir_pages_baseAddr);
    
  return PID;
}

void sys_exit()
{  
}

int sys_gettime()
{
  return zeos_ticks;
}

int sys_write(int fd, char *buffer, int size)
{
  int error_fd =  check_fd(fd, ESCRIPTURA);
  if(error_fd < 0) return error_fd;

  if (buffer == NULL) {
    return -EFAULT;
  }

  if (size < 0) {
    return -EINVAL;
  }

  if (access_ok(VERIFY_READ, buffer, size) == 0) {
    return -EFAULT;
  }

  int bytes = size;
  int w_bytes; 
  int offset = 0; 
  int current_size;

  while (bytes > 0) {

      if (bytes > BLOCK) {
        current_size = BLOCK;
      }
      else {
        current_size = bytes;
      }

      if (copy_from_user(buffer + offset, buff, current_size) != 0) {
          return -EFAULT;  // Si ocurre un error al copiar, devolvemos un error
      }

      // Escribir en consola
      w_bytes = sys_write_console(buff, current_size);
      if (w_bytes < 0) {
          return -EIO;
      }

      offset += current_size;
      bytes -= w_bytes;  
  }
  return size - bytes;  // Devuelve el número de bytes escritos
}
