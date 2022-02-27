#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "threads/thread.h"

void syscall_init(void);

void check_valid(const void *);

void halt(void);
void exit(int );
tid_t exec(const char *);
int wait(int );

int read(int fd, const void *, unsigned );
int write(int fd, const void *, unsigned );

bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);

int fibonacci(int n);
int max_of_four_int(const int a, const int b, const int c, const int d);

#endif /* userprog/syscall.h */
