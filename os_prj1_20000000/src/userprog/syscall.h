#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "threads/thread.h"

void syscall_init(void);

void check_valid(const void *);

void sys_halt(void);
void sys_exit(int );
tid_t sys_exec(const char *);
int sys_wait(int );
int sys_write(int fd, const void *, unsigned );
int sys_read(int fd, const void *, unsigned );

int sys_fibonacci(int n);
int sys_max_of_four_int(const int a, const int b, const int c, const int d);

#endif /* userprog/syscall.h */
