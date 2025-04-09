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
extern struct list_head ready_queue;
extern struct list_head blocked;

// union task_union * child_union_global;
// extern struct task_struct * father_struct;

extern unsigned int get_ebp();

int PID_global = 1000;

int check_fd(int fd, int permissions)
{
  if (fd != 1)
    return -9; /*EBADF*/
  if (permissions != ESCRIPTURA)
    return -13; /*EACCES*/
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

int ret_from_fork()
{
  return 0;
}

int sys_fork (void) {
  int PID = -1;

  if (!list_empty(&free_queue)) {
      struct list_head *lh = list_first(&free_queue);
      list_del(lh);
      struct task_struct *ts = list_head_to_task_struct(lh);

      copy_data(current(), ts, sizeof(union task_union));
      allocate_DIR(ts);

      int avail_frames[NUM_PAG_DATA];

      for (int i = 0; i < NUM_PAG_DATA; ++i) {
          avail_frames[i] = alloc_frame();
          if (avail_frames[i] < 0) {
              for (int j = 0; j <= i; ++j) {
                  free_frame(avail_frames[j]);
              }
              list_add_tail(&ts->list, &free_queue);
              return -1;
          }
      }

      for (int i = 0; i < NUM_PAG_KERNEL; ++i) {
          set_ss_pag(get_PT(ts), i, get_frame(get_PT(current()), i));
      }

      for (int i = 0; i < NUM_PAG_DATA; ++i) {
          set_ss_pag(get_PT(ts), i + NUM_PAG_KERNEL, avail_frames[i]);
          set_ss_pag(get_PT(current()), i + NUM_PAG_KERNEL + NUM_PAG_DATA + NUM_PAG_CODE, avail_frames[i]);
          copy_data((void*)((i + NUM_PAG_KERNEL) << 12), (void*)((i + NUM_PAG_KERNEL + NUM_PAG_DATA + NUM_PAG_CODE) << 12), PAGE_SIZE);
          del_ss_pag(get_PT(current()), i + NUM_PAG_KERNEL + NUM_PAG_DATA + NUM_PAG_CODE);
      }

      for (int i = 0; i < NUM_PAG_CODE; ++i) {
          set_ss_pag(get_PT(ts), i + NUM_PAG_KERNEL + NUM_PAG_DATA , get_frame(get_PT(current()), i + NUM_PAG_KERNEL + NUM_PAG_DATA ));
      }

      set_cr3(get_DIR(current()));

      

      PID = PID_global++;
      ts->PID = PID;
      ts->father = current();
      ts->pending_unblocks = 0;
      INIT_LIST_HEAD(&ts->anchor);
      INIT_LIST_HEAD(&ts->childs);

      union task_union *tu = (union task_union*) ts;

      tu->stack[KERNEL_STACK_SIZE - 19] = (unsigned long) 0;

      tu->stack[KERNEL_STACK_SIZE - 18] = (unsigned long) ret_from_fork;

      ts->kernel_esp = (unsigned long) &tu->stack[KERNEL_STACK_SIZE - 19];

      list_add_tail(&ts->list, &ready_queue);

      
      list_add_tail(&ts->anchor, &(current()->childs));

      return PID; 
  }
  
  return -1;
}

void sys_exit()
{
  page_table_entry *PT = get_PT(current());

  //Desalocar las páginas de datos del proceso
  for (int i=0; i<NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(PT, PAG_LOG_INIT_DATA+i));
    del_ss_pag(PT, PAG_LOG_INIT_DATA+i);
  }
  
  //Free task_struct
  list_add_tail(&(current()->list), &free_queue);

  //Dar valor inválido de PID
  current()->PID=-1;
  current()->dir_pages_baseAddr = NULL;
  current()->father = NULL;
  
  //Liberar la memoria de los hijos
  //Recorremos la lista de hijos y liberamos su memoria
  //Eliminamos el padre de la lista de hijos
  struct list_head * e = list_first(&(current()->childs));
    if (!list_empty(&(current()->childs))) {
        list_for_each(e, &(current()->childs)) {
            struct task_struct* t = list_head_to_task_struct(e);
            t->father = NULL;
            list_del(&t->anchor);
        }
    }

	update_process_state_rr(current(), &free_queue);
	sched_next_rr();
}

int sys_gettime()
{
  return zeos_ticks;
}

int sys_write(int fd, char *buffer, int size)
{
  int error_fd = check_fd(fd, ESCRIPTURA);
  if (error_fd < 0)
    return error_fd;

  if (buffer == NULL)
  {
    return -EFAULT;
  }

  if (size < 0)
  {
    return -EINVAL;
  }

  if (access_ok(VERIFY_READ, buffer, size) == 0)
  {
    return -EFAULT;
  }

  int bytes = size;
  int w_bytes;
  int offset = 0;
  int current_size;

  while (bytes > 0)
  {

    if (bytes > BLOCK)
    {
      current_size = BLOCK;
    }
    else
    {
      current_size = bytes;
    }

    if (copy_from_user(buffer + offset, buff, current_size) != 0)
    {
      return -EFAULT; // Si ocurre un error al copiar, devolvemos un error
    }

    // Escribir en consola
    w_bytes = sys_write_console(buff, current_size);
    if (w_bytes < 0)
    {
      return -EIO;
    }

    offset += current_size;
    bytes -= w_bytes;
  }
  return size - bytes; // Devuelve el número de bytes escritos
}

void sys_block(void) {
  if(current() != init_task) {
    current()->pending_unblocks = current()->pending_unblocks -1;
    if (current()->pending_unblocks <= 0) {
      current()->pending_unblocks = 1;
      update_process_state_rr(current(), &blocked);
      sched_next_rr();
    } 
  }
}

int sys_unblock(int pid) {
    struct list_head *tmp = &(current()->childs);
    struct list_head *e = tmp->next;

    while (e != tmp) {
        struct task_struct* t = list_entry(e, struct task_struct, anchor);
        
        if (t->PID == pid && t->pending_unblocks > 0) {
            update_process_state_rr(t, &ready_queue);
            return 0;
        }
        else if (t->PID == pid) {
            t->pending_unblocks++;
        }

        e = e->next; // Avanzar al siguiente elemento
    }
    return 0;
}