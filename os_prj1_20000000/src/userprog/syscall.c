#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
    /* Project 1: system call impelementation */
    switch (*(uint32_t*)(f->esp)) {
    case SYS_HALT:
        sys_halt();
        break;

    case SYS_EXIT:
        check_valid(f->esp + 4);
        sys_exit(*(uint32_t *)(f->esp + 4));
        break;

    case SYS_EXEC:
        check_valid(f->esp + 4);
        f->eax = sys_exec((const char *)*(uint32_t *)(f->esp + 4));
        break;

    case SYS_WAIT:
        check_valid(f->esp + 4);
        f->eax = sys_wait(*(uint32_t *)(f->esp + 4));
        break;

    case SYS_WRITE:
        check_valid(f->esp + 20);
        check_valid(f->esp + 24);
        check_valid(f->esp + 28);
        f->eax = sys_write(*(int*)(f->esp + 20), *(void**)(f->esp + 24), *(unsigned*)(f->esp + 28));
        break;

    case SYS_READ:
        check_valid(f->esp + 20);
        check_valid(f->esp + 24);
        check_valid(f->esp + 28);
        f->eax = sys_read(*(int*)(f->esp + 20), *(void**)(f->esp + 24), *(unsigned*)(f->esp + 28));
        break;

    case SYS_FIBONACCI:
        f->eax = sys_fibonacci(*(int*)(f->esp + 4));
        break;

    case SYS_MAX_OF_FOUR_INT:
        f->eax = sys_max_of_four_int(*(int*)(f->esp + 4), *(int*)(f->esp + 8), *(int*)(f->esp + 12), *(int*)(f->esp + 16));
        break;

    default:
        break;
    }

}

void check_valid(const void *vaddr) {
    if (!is_user_vaddr(vaddr))
        sys_exit(-1);
}

void sys_halt() {
    shutdown_power_off();
}

void sys_exit(int status) {
    printf("%s: exit(%d)\n", thread_name(), status);
    thread_current()->exit_status = status;
    thread_exit();
}

tid_t sys_exec(const char *arg) {
    return process_execute(arg);
}

int sys_wait(int pid) {
    return process_wait((tid_t) pid);
}

int sys_write(int fd, const void *buffer, unsigned size) {
    if (fd == 1) {
        putbuf((char*)buffer, (size_t)size);
        return (int)size;
    }

    return -1;
}

int sys_read(int fd, const void *buffer, unsigned size) {
    if (fd == 0) {
        for (int i = 0; i < (int)size && buffer != '\0'; i++)
            buffer = input_getc();

        return (int)size;
    }

    return -1;
}

int sys_fibonacci(int n) {
    int curr = 0, next = 1;
    int temp;
    
    for (int i = 0; i < n; i++) {
        temp = next;
        next = curr + next;
        curr = temp;
    }

    return curr;
}

int sys_max_of_four_int(const int a, const int b, const int c, const int d) {
    int max1 = a > b ? a : b;
    int max2 = c > d ? c : d;
    
    int res = max1 > max2 ? max1 : max2;
    return res;
}
