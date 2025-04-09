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
  // Comprobamos si hay procesos libres en la cola de procesos libres
  if (!list_empty(&free_queue)) {
      // Obtenemos el primer elemento de la cola de procesos libres
      struct list_head *l = list_first(&free_queue);
      list_del(l); // Lo eliminamos de la cola
      struct task_struct *t = list_head_to_task_struct(l); // Obtenemos la estructura del proceso

      // Copiamos los datos del proceso actual al nuevo proceso
      copy_data(current(), t, sizeof(union task_union));
      allocate_DIR(t);

      int pages[NUM_PAG_DATA]; // Array para almacenar los frames asignados

      // Asignamos frames para las páginas de datos del proceso hijo
      for (int i = 0; i < NUM_PAG_DATA; ++i) {
          pages[i] = alloc_frame();
          if (pages[i] < 0) { // Si no se pueden asignar frames, liberamos los ya asignados
              for (int j = 0; j <= i; ++j) {
                  free_frame(pages[j]);
              }
              list_add_tail(&t->list, &free_queue); // Devolvemos el proceso a la cola de libres
                return -ENOMEM; // Error al asignar frames
          }
      }

      // Compartimos las páginas de kernel entre el padre y el hijo
      for (int i = 0; i < NUM_PAG_KERNEL; ++i) {
          set_ss_pag(get_PT(t), i, get_frame(get_PT(current()), i));
      }

      // Asignamos las páginas de datos al hijo y copiamos los datos del padre
      for (int i = 0; i < NUM_PAG_DATA; ++i) {
          set_ss_pag(get_PT(t), i + NUM_PAG_KERNEL, pages[i]);                                                                          // Asignamos la página al hijo
          set_ss_pag(get_PT(current()), i + NUM_PAG_KERNEL + NUM_PAG_DATA + NUM_PAG_CODE, pages[i]);                                    // Temporalmente al padre
          copy_data((void*)((i + NUM_PAG_KERNEL) << 12), (void*)((i + NUM_PAG_KERNEL + NUM_PAG_DATA + NUM_PAG_CODE) << 12), PAGE_SIZE); // Copiamos los datos
          del_ss_pag(get_PT(current()), i + NUM_PAG_KERNEL + NUM_PAG_DATA + NUM_PAG_CODE);                                              // Eliminamos la asignación temporal
      }

      // Compartimos las páginas de código entre el padre y el hijo
      for (int i = 0; i < NUM_PAG_CODE; ++i) {
          set_ss_pag(get_PT(t), i + NUM_PAG_KERNEL + NUM_PAG_DATA , get_frame(get_PT(current()), i + NUM_PAG_KERNEL + NUM_PAG_DATA ));
      }

      set_cr3(get_DIR(current())); // Restauramos el directorio de páginas del padre con un flush del TLB

      // Inicializamos los valores del proceso hijo
      t->PID = PID_global++; 
      t->father = current(); 
      t->pending_unblocks = 0; 
      INIT_LIST_HEAD(&t->anchor); 
      INIT_LIST_HEAD(&t->childs);

      // Configuramos la pila del kernel para el proceso hijo
      union task_union *tu = (union task_union*) t;
      tu->stack[KERNEL_STACK_SIZE - 19] = (unsigned long) 0;              // Valor de retorno
      tu->stack[KERNEL_STACK_SIZE - 18] = (unsigned long) ret_from_fork;  // Dirección de retorno
      t->kernel_esp = (unsigned long) &tu->stack[KERNEL_STACK_SIZE - 19]; // Apuntamos al nuevo ESP

      // Añadimos el proceso hijo a la cola de listos y a la lista de hijos del padre
      list_add_tail(&t->list, &ready_queue);
      list_add_tail(&t->anchor, &(current()->childs));

      return t->PID;
  }
  
  return -EAGAIN; // Error si no hay procesos libres
}

void sys_exit()
{
  if(sys_getpid() == 1) return; //Comprobamos que no es el proceso init
    struct task_struct* t = current();
	  page_table_entry *entry = t->dir_pages_baseAddr;

  //Liberar memoria
	for (int i = 0; i < NUM_PAG_DATA; ++i) {
		free_frame(get_frame(entry, i + NUM_PAG_KERNEL));
		del_ss_pag(entry, i + NUM_PAG_KERNEL);
	}

	t->PID = -1;
	t->dir_pages_baseAddr = NULL;
  t->father = NULL;

  if (t->anchor.next == NULL && t->anchor.prev == NULL) list_del(&t->anchor);

  struct list_head * e = list_first(&(current()->childs));

  if (!list_empty(&(current()->childs))) {
    list_for_each(e, &(current()->childs)) {
      struct task_struct* ts = list_head_to_task_struct(e);
      ts->father = NULL;
      list_del(&ts->anchor);
    }
  }

	update_process_state_rr(t, &free_queue);
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