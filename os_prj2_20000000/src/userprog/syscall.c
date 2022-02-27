#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

#include "devices/input.h"
#include "devices/shutdown.h"

//#include "filesys/off_t.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *);

/* prj2 =================== */
struct lock file_lock;
/* ======================== */

void
syscall_init (void)
{
  /* prj2 =================== */
  lock_init(&file_lock);
  /* ======================== */
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
    /* Project 1: system call impelementation */
    switch (*(uint32_t*)(f->esp)) {
    case SYS_HALT:
        halt();
        break;

    case SYS_EXIT:
        check_valid(f->esp + 4);
        exit(*(uint32_t *)(f->esp + 4));
        break;

    case SYS_EXEC:
        check_valid(f->esp + 4);
        f->eax = exec((const char *)*(uint32_t *)(f->esp + 4));
        break;

    case SYS_WAIT:
        check_valid(f->esp + 4);
        f->eax = wait(*(uint32_t *)(f->esp + 4));
        break;

    case SYS_READ:
        check_valid(f->esp + 4);
        check_valid(f->esp + 8);
        check_valid(f->esp + 12);
        f->eax = read((int)*(uint32_t *)(f->esp + 4), (void*)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
        break;

    case SYS_WRITE:
        check_valid(f->esp + 4);
        check_valid(f->esp + 8);
        check_valid(f->esp + 12);
        f->eax = write((int)*(uint32_t *)(f->esp + 4), (void*)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
        break;

    /* Project 2: system call impelementation */
    case SYS_CREATE:
        check_valid(f->esp + 4);
        check_valid(f->esp + 8);
        f->eax = create((const char *)*(uint32_t*)(f->esp + 4), (unsigned)*(uint32_t*)(f->esp + 8));
        break;
    
    case SYS_REMOVE:
        check_valid(f->esp + 4);
        f->eax = remove((const char *)*(uint32_t*)(f->esp + 4));
        break;

    case SYS_OPEN:
        check_valid(f->esp + 4);
        f->eax = open((const char *)*(uint32_t*)(f->esp + 4));
        break;
    
    case SYS_FILESIZE:
        check_valid(f->esp + 4);
        f->eax = filesize((int)*(uint32_t*)(f->esp + 4));
        break;

    case SYS_SEEK:
        check_valid(f->esp + 4);
        check_valid(f->esp + 8);
        seek((int)*(uint32_t*)(f->esp + 4), (unsigned)*(uint32_t*)(f->esp + 8));
        break;
    
    case SYS_TELL:
        check_valid(f->esp + 4);
        f->eax = tell((int)*(uint32_t*)(f->esp + 4));
        break;
    
    case SYS_CLOSE:
        check_valid(f->esp + 4);
        close((int)*(uint32_t*)(f->esp + 4));
        break;

    /* Project 1: additional system call impelementation */
    case SYS_FIBONACCI:
        f->eax = fibonacci(*(int*)(f->esp + 4));
        break;

    case SYS_MAX_OF_FOUR_INT:
        f->eax = max_of_four_int(*(int*)(f->esp + 4), *(int*)(f->esp + 8), *(int*)(f->esp + 12), *(int*)(f->esp + 16));
        break;

    default:
        break;
    }

}

void check_valid(const void *vaddr) {
    if (!is_user_vaddr(vaddr))
        exit(-1);
    if (vaddr == NULL)
        exit(-1);
}

void halt() {
    shutdown_power_off();
}

void exit(int status) {
    printf("%s: exit(%d)\n", thread_name(), status);
    thread_current()->exit_status = status;
    // /* =========prj2========== */
    // for (int i = 0; i < 128; i++) {
    //     if (thread_current()->file_d[i] != NULL)
    //         close(i);
    // }
    // /* =================== */
    //if we call thread_exit() -> process_exit()
    thread_exit();
}

tid_t exec(const char *cmd_line) {
    return process_execute(cmd_line);
}

int wait(int pid) {
    return process_wait((tid_t) pid);
}

int read(int fd, const void *buffer, unsigned size) {
    /* =========modified in prj2========== */
    int i = -1;
    struct thread* cur = thread_current();

    check_valid(buffer);
    lock_acquire(&file_lock);

    if (fd == 0) { //read from the keyboard
        for (int i = 0; i < (int)size && ((char*)buffer)[i] != '\0'; i++)
            buffer = input_getc();
    }

    else if (2 < fd && fd < 128) {
        if(cur->file_d[fd] == NULL) {
            lock_release(&file_lock);
            return -1;
        }
        i = (int)file_read(cur->file_d[fd], (void*)buffer, (off_t)size);
    }

    lock_release(&file_lock);
    return i;
    /* =================== */
}

int write(int fd, const void *buffer, unsigned size) {
    /* =========prj2========== */
    struct thread* cur = thread_current();
    int i = -1;

    check_valid(buffer);
    lock_acquire(&file_lock);

    if (fd == 1) { //write to the console
        putbuf((char*)buffer, (size_t)size);
        i = (int) size;
    }

    else if (2 < fd && fd < 128) {
        if (cur->file_d[fd] == NULL) {
            lock_release(&file_lock);
            exit(-1);
        }
        i = (int)file_write(cur->file_d[fd], buffer, (off_t)size);
    }

    lock_release(&file_lock);
    return i;
    /* =================== */
}

/* ================ project 2 =================== */
bool create(const char *file, unsigned initial_size) {
    check_valid(file);
    return filesys_create(file, (off_t)initial_size);
}

bool remove(const char *file) {
    check_valid(file);
    return filesys_remove(file);
}

int open(const char *file) {
    struct file* fp;
    int fd;

    check_valid(file);
    lock_acquire(&file_lock); //lock

    fp = filesys_open(file);

    if (fp == NULL) {
        lock_release(&file_lock); //lock
        return -1;
    }

    if (strcmp(thread_name(), file) == 0)
        file_deny_write(fp);
    
    for (fd = 3; fd < 128; fd++) {
        if (thread_current()->file_d[fd] == NULL) {
            thread_current()->file_d[fd] = fp;
            lock_release(&file_lock); //lock
            return fd;
        }
    }

    lock_release(&file_lock); //lock
    return -1;
}

int filesize(int fd) {
    return file_length(thread_current()->file_d[fd]);
}

void seek(int fd, unsigned position) {
    file_seek(thread_current()->file_d[fd], position);
}

unsigned tell(int fd) {
    return file_tell(thread_current()->file_d[fd]);
}

void close(int fd) {
    struct file* fp;
    fp = thread_current()->file_d[fd];
    if (fp == NULL)
        exit(-1);
    thread_current()->file_d[fd] = NULL;
    file_close(fp);
}
/* ============================================== */

/* ================ project 1 =================== */
int fibonacci(int n) {
    int curr = 0, next = 1;
    int temp;
    
    for (int i = 0; i < n; i++) {
        temp = next;
        next = curr + next;
        curr = temp;
    }

    return curr;
}

int max_of_four_int(const int a, const int b, const int c, const int d) {
    int max1 = a > b ? a : b;
    int max2 = c > d ? c : d;
    
    int res = max1 > max2 ? max1 : max2;
    return res;
}
/* ============================================== */