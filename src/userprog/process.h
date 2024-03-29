#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
struct plist plist;
void process_init (void);
void process_print_list (void);
void process_exit (int status);
tid_t process_execute (const char *file_name);
int process_wait (tid_t);
bool erase(pid_t k UNUSED, struct process_information*, int aux UNUSED);
void cleanup_children(pid_t, struct process_information* , int);
void process_cleanup (void);
void process_activate (void);

/* This is unacceptable solutions. */
#define INFINITE_WAIT() for ( ; ; ) thread_yield()
#define BUSY_WAIT(n)       \
    do {                   \
      int i = n;           \
      while ( i --> 0 )    \
        thread_yield();    \
    } while ( 0 )

#endif /* userprog/process.h */
