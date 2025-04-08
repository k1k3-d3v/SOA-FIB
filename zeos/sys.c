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

int sys_fork()
{
  // Comprobar si hay espacio en la cola de procesos libres
  if (list_empty(&free_queue))
    return -ENOMEM;

  // a) Asignar primer task_struct libre
  struct list_head *e = list_first(&free_queue);
  list_del(e);

  struct task_struct *child_struct = list_head_to_task_struct(e);
  union task_union *child_union = (union task_union *)child_struct;

  // b) Copiamos el PCB del padre al hijo
  copy_data(current(), child_union, sizeof(union task_union));

  // c) Asignamos un directorio al hijo
  allocate_DIR(child_struct);

  page_table_entry *child_PT = get_PT(child_struct);

  // d) Buscando páginas libres para la pila y código de usuario del hijo
  int pages[NUM_PAG_DATA];

  for (int i = 0; i < NUM_PAG_DATA; i++)
  {
    pages[i] = alloc_frame();

    if (pages[i] == -1)
    {
      for (int j = 0; j < i; j++)
      {
        free_frame(pages[j]);
      }

      list_add_tail(&child_struct->list, &free_queue);
      return -ENOMEM;
    }
  }

  page_table_entry *parent_PT = get_PT(current());

  // e) Mappear las páginas de sistema, codifo y datos + pila de usuario ()
  for (int i = 0; i < NUM_PAG_KERNEL; i++)
  {
    set_ss_pag(child_PT, i, get_frame(parent_PT, i));
  }

  for (int i = 0; i < NUM_PAG_CODE; i++)
  {
    set_ss_pag(child_PT, PAG_LOG_INIT_CODE + i, get_frame(parent_PT, PAG_LOG_INIT_CODE + i));
  }

  for (int i = 0; i < NUM_PAG_DATA; i++)
  {
    set_ss_pag(child_PT, PAG_LOG_INIT_DATA + i, pages[i]);
  }

  // f) Heredar datos + pila del padre (Mediante creacion de páginas temporales)
  for (int i = NUM_PAG_KERNEL + NUM_PAG_CODE; i < NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA; i++)
  {
    set_ss_pag(parent_PT, i + NUM_PAG_DATA, get_frame(child_PT, i));             // Asignamos la pagina fisica del hijo al padre
    copy_data((void *)(i << 12), (void *)((i + NUM_PAG_DATA) << 12), PAGE_SIZE); // Convertimos el número de página a dirección física
    del_ss_pag(parent_PT, i + NUM_PAG_DATA);                                     // Eliminamos la página temporal creada en el padre
  }

  set_cr3(get_DIR(current())); // Forzamos un flush de la TLB para eliminar los accesos del padre a las páginas del hijo

  // g) Asignar PID al hijo != posición en el vector de tareas
  child_struct->PID = PID_global++;

  // h) Inicializar campos task_struct del hijo
  child_struct->quantum = 5;
  child_struct->father = current(); // Asignamos el padre al hijo
  child_struct->pending_unblocks = 0; // Inicializamos el número de bloqueos pendientes del hijo a 0
  INIT_LIST_HEAD(&child_struct->anchor);
  INIT_LIST_HEAD(&child_struct->childs);

  // i) Preparar la pila del hijo para task_switch
  child_union->stack[KERNEL_STACK_SIZE - 18] = (unsigned long)&ret_from_fork;              // Establecemos la dirección de retorno de la función fork
  child_union->stack[KERNEL_STACK_SIZE - 19] = 0;                                          // Establecemos el fake_ebp en 0
  child_struct->kernel_esp = (unsigned long)&(child_union->stack[KERNEL_STACK_SIZE - 19]); // Hacemos que kernel_esp apunte al tope de la pila

  // j) Añadir hijo a la cola de listos
  list_add_tail(&child_struct->list, &ready_queue);

  //Añadir el hijo a la lista de hijos del padre
  list_add_tail(&child_struct->anchor, &(current()->childs));

  // k) Devolver PID del hijo
  return child_struct->PID;
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