/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1
#define BLOCK 128

char buff[128];

extern int zeos_ticks;

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
    return -1; // Cómo sabemos el código del error?
  }
  if (size <= 0) {
    return -1; // Cómo sabemos el código del error=
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
          return -1;  // Si ocurre un error al copiar, devolvemos un error
      }

      // Escribir en consola
      w_bytes = sys_write_console(buff, BLOCK);
      if (w_bytes < 0) {
          return w_bytes;  
      }

      offset += current_size;
      bytes -= w_bytes;  
  }
  return size - bytes;  // Devuelve el número de bytes escritos
}
