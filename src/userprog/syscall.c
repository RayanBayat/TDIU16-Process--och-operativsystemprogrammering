#include <stdio.h>
#include "stdio.h"
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"

#include "devices/timer.h"
#include "userprog/flist.h"
#include "threads/init.h"

static void syscall_handler (struct intr_frame *);
static bool verify_fix_length(void* start, unsigned length);
static bool verify_variable_length(char* start);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:
   
   int sys_read_arg_count = argc[ SYS_READ ];
   
   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
  /* basic calls */
  0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1, 
  /* not implemented */
  2, 1,    1, 1, 2, 1, 1,
  /* extended, you may need to change the order of these two (plist, sleep) */
  0, 1
  };
  static void sys_halt(void)
  {
   // printf("System Call Halt\n");
    power_off();
  }
  static void sys_exit(int status)
  {
  //  printf("System Call Exit: thread: %s#%d\n", thread_name(), thread_tid());
   // printf("Status: %d\n", status);
    process_exit(status);
    
    thread_exit();
    return;
  }
  static void
  syscall_handler (struct intr_frame *f) 
  {


    int32_t* esp = (int32_t*)f->esp;
    //printf("esp[0] = %d\n", esp[0]);
   // printf("argc : %d\n",sizeof(char*) * (argc[esp[0]] + 1));
   if(!verify_variable_length((char*)esp))
   {
    sys_exit(-1);
   }
   if(!verify_fix_length(esp, sizeof(int32_t*)))
       sys_exit(-1);
   
    if(esp[0] > SYS_NUMBER_OF_CALLS || esp[0] < 0)
      {
        sys_exit(-1);
      }
    
    
    
  if(!verify_fix_length(esp, sizeof(char*) * (argc[esp[0]] + 1)))
    sys_exit(-1); 

  switch ( esp[0]/* retrive syscall number */ )
  {
    case SYS_EXEC:
    {
      char* file_name = (char*)esp[1];

      if(!verify_variable_length(file_name))
        thread_exit();

      f->eax = process_execute(file_name);
      return;
    }
    case SYS_WAIT:
    {
      f->eax = process_wait(esp[1]);
      return;
    }
    case SYS_PLIST: 
    {
      plist_printf(&plist);
      return;
    }
    case SYS_SLEEP:
    {
      timer_msleep(esp[1]);
      return;
    }
    case SYS_HALT:
    {
     
      sys_halt();
      break;
    }
    case SYS_EXIT:
    {
      
    sys_exit(esp[1]);
     break;
    }
    case SYS_CREATE:
    {
      char* file_name = (char*)esp[1];
      int file_size = esp[2];
      if(file_name == NULL || !verify_fix_length(file_name, file_size))
      {
        sys_exit(-1);
        return;
      }
      f->eax = filesys_create(file_name, file_size);

      return;
    }
    case SYS_OPEN:
    {
      char* file_name = (char*)esp[1];
      int fd = 0;

      if(file_name == NULL || !verify_variable_length(file_name))
      sys_exit(-1);

      if (file_name == NULL)
      {
        sys_exit(-1);
      }
      struct file* file = filesys_open(file_name);

      if(file == NULL)
      {
        f->eax = -1;
        return;
      }
      
      

      fd = map_insert(&(thread_current()->fmap),file);
      //printf("fd = %d\n", fd);
      f->eax = fd;

      return;
    }
    case SYS_REMOVE:
    {
       char* file_name = (char*)esp[1];
       f->eax = filesys_remove(file_name);

        return;

    }
    case SYS_CLOSE:
    {
      int fd = esp[1];
      struct file* file = map_remove(&(thread_current()->fmap), fd);

      if(file == NULL)
      {
        f->eax = -1;
        return;
      }
      filesys_close(file);
      return;
    }
    case SYS_READ:
    {
      //process_cleanup();
      int serviced = 0;
      //printf("Hello I'm reading \n");
      int fd= esp[1];
      char* buffer = (char*)esp[2];
      int length = esp[3];

      if(!verify_fix_length(buffer, length))
      {
       
        sys_exit(-1);
        return;
      }

      if(fd == STDOUT_FILENO || buffer == NULL)
      {
        f->eax = -1;
        return;
      }

     // printf("fd %d\n", fd);
      //printf("buffer %c\n", *buffer);
      //printf("length %d\n",length);
      if (fd == STDIN_FILENO)
      {
        for (int i = 0; i < length; i++)
        {
          //printf("input_getc() is next\n");
          buffer[i] = input_getc();
        //  printf("input_getc() is done\n");
          if (buffer[i] == '\r')
          {
            buffer[i] = '\n';
          }
          
          putbuf(&buffer[i],1);
          serviced++;
          
        }
        
      }
      else
      {
       struct file* file = map_find(&(thread_current()->fmap), fd);
        if(file == NULL)
        {
          f->eax = -1;
          return;
        }

        serviced = file_read(file, buffer, length); 
      }
      
      //printf("returning \n");
      f->eax = serviced;
      return;
    }
    case SYS_WRITE:
    {
      
      int fd = esp[1];
      char* buffer = (char*)esp[2];
      int length = esp[3];
    //  printf("\n");
       //printf("Hello I'm writing \n");
      if(!verify_fix_length(buffer, length))
      {
       
        sys_exit(-1);
        return;
      }
      if(fd == STDIN_FILENO || buffer == NULL)
      {
        f->eax = -1;
        return;
      }
      if (fd == STDOUT_FILENO)
      {

        putbuf(buffer,length);
        // for (int i = 0; i < length; i++)
        // {
        //   putbuf(&buffer[i],1);
        //   serviced++;
        // }
        
      }
      else
      {
        struct file* file = map_find(&(thread_current()->fmap), fd);
        if(file == NULL)
        {
          f->eax = -1;
          return;
        }
        length = file_write(file, buffer, length);
      }

      f->eax = length;
     // printf("Write done\n");
      return;
    }
      case SYS_SEEK:
      {
       // printf("this is seek\n");

        int fd = esp[1];
        int position = esp[2];
        struct file* file = map_find(&(thread_current()->fmap),fd) ;
        if(file == NULL)
        {
          f->eax = -1;
          return;
        }
        if (position >= (int)file_length(file))
        {
          position = (int)file_length(file);
        }
        
        file_seek(file,position);

        // printf("seek done\n");
        return;
      }
      case SYS_TELL:
      {
       //  printf("this is tell\n");
        int fd = esp[1];
        struct file* file = map_find(&(thread_current()->fmap),fd) ;
        if(file == NULL)
        {
          f->eax = -1;
          return;
        }
        f->eax = file_tell(file);
        //printf("tell done\n");
        return;
      
      }
      case SYS_FILESIZE:
      {
          int fd = esp[1];
        struct file* file = map_find(&(thread_current()->fmap),fd) ;
        f->eax = file_length(file);
       
        return;

      }
    default:
    {
      printf ("Executed an unknown system call!\n");
      
      printf ("Stack top + 0: %d\n", esp[0]);
      printf ("Stack top + 1: %d\n", esp[1]);
      
      thread_exit ();
    }

  }
  
}
bool verify_fix_length(void* start, unsigned length)
{
  if(!is_user_vaddr(start))
  return false;

	void *cur, *end;
	end = (void*)((unsigned)start + length);
	cur = pg_round_down(start);

	while(cur < end)
	{
    if(!is_user_vaddr(cur))
      return false;
		if(pagedir_get_page(thread_current()->pagedir, cur) == NULL)
			return false;

		cur = (void*)((unsigned)cur + PGSIZE); /* GOTO next page */
	}
	return true;
}

/* Verify all addresses from and including 'start' up to and including
 * the address first containg a null-character ('\0'). (The way
 * C-strings are stored.)
 */
bool verify_variable_length(char* start)
{
  if(!is_user_vaddr(start))
  return false;

  if(pagedir_get_page(thread_current()->pagedir, start) == NULL)
    return false;
 
  char *cur = start;

  while(*cur != '\0')
  {
    unsigned prev_pg = pg_no(cur++);


    if(pg_no(cur) != prev_pg)
     {
      if(!is_user_vaddr(cur))
      return false;

      if(pagedir_get_page(thread_current()->pagedir, cur) == NULL)
        return false;
     }
  }

  return true;
}
