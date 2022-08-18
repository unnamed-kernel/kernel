#include <kernel/logging.h>
#include <kernel/sched.h>
#include <kernel/com.h>
#include <kernel/pmm.h>
#include <kernel/arch.h>
#include <kernel/utils.h>
#include <kernel/loader.h>

#include <stdint.h>
#include <unistd.h>

#include "syscall.h"
#include "gdt.h"
#include "regs.h"
#include "asm.h"

typedef int64_t (*syscall_t)(regs_t *);

void syscall_init(void)
{
    klog(INFO, "Initializing system calls");
    asm_write_msr(MSR_EFER, asm_read_msr(MSR_EFER) | 1);
    asm_write_msr(MSR_STAR, ((uint64_t)(GDT_KERNEL_CODE * 8) << STAR_KCODE_OFFSET) | ((uint64_t) (((GDT_USER_DATA - 1) * 8) | 3 ) << STAR_UCODE_OFFSET));
    asm_write_msr(MSR_LSTAR, (uint64_t) syscall_handle);
    asm_write_msr(MSR_SYSCALL_FLAG_MASK, 0xfffffffe);
    klog(OK, "System calls initialized");
}

int64_t syscall_log(regs_t *regs)
{
    klog_lock();
    com_puts((char const *) regs->rbx);
    klog_unlock();
    return 0;
}

int64_t syscall_getpid(__attribute__((unused)) regs_t *regs)
{
    return sched_current()->pid;
}

int64_t syscall_alloc(regs_t *regs)
{
    return (uintptr_t) malloc(regs->rbx);
}

int64_t syscall_realloc(regs_t *regs)
{
    return (uintptr_t) realloc((void *) regs->rbx, regs->rcx);
}

int64_t syscall_free(regs_t *regs)
{
    free((void *) regs->rbx);
    return 0;
}

syscall_t syscall_matrix[] = {
    [SYS_LOG] = syscall_log,
    [SYS_GETPID] = syscall_getpid,
    [SYS_ALLOC] = syscall_alloc,
    [SYS_FREE] = syscall_free,
    [SYS_REALLOC] = syscall_realloc,
};

int64_t syscall_handler(regs_t *regs)
{
    return syscall_matrix[regs->rax](regs);
}
