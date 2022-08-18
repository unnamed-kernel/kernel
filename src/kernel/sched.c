#include "vec.h"
#ifdef __x86_64__ 
#include <arch/x86_64/cpu.h>
#endif /* !__x86_64__ */

#include <stdlib.h>

#include <stddef.h>
#include <assert.h>

#include "arch.h"
#include "logging.h"
#include "sched.h"
#include "pmm.h"
#include "utils.h"
#include "loader.h"
#include "klock.h"

static pid_t last_pid = 0;
static bool sched_running = false;

DECLARE_LOCK(sched);

void sched_init(void)
{
    LOCK(sched);

    task_t *boot = task_create(vmm_get_kernel_pml(), "/boot/kernel.elf", 0);
    boot->pid = 0;

    for (size_t i = 0; i < cpu_get_count(); i++)
    {
        vec_push(&cpu(i)->tasks, boot);
    }

    sched_running = true;

    UNLOCK(sched);
}

void sched_yield(regs_t *regs)
{
    if (cpu_self()->tick++ > __SCHED_QUANTUM__ && cpu_self()->tasks.length > 1)
    {
        cpu_self()->tick = 0;
        task_t *current_task = cpu_self()->tasks.data[cpu_self()->current];
        context_save(&current_task->context, regs);
        assert(current_task->context.regs.rip != 0);

        for (;;)
        {
            cpu_self()->current++;

            if (cpu_self()->current >= cpu_self()->tasks.length)
            {
                cpu_self()->current = 0;
            }

            current_task = cpu_self()->tasks.data[cpu_self()->current];

            if (current_task->state == TASK_READY)
            {
                break;
            }
        }

        context_switch(&current_task->context, regs);
        vmm_switch_space(current_task->space);
    }
}

void sched_push(task_t *task)
{
    LOCK(sched);

    task->pid = ++last_pid;

    size_t smallest = cpu(0)->tasks.length;
    size_t cpu_id = 0;

    for (size_t i = 0; i < cpu_get_count(); i++)
    {
        if (cpu(i)->tasks.length < smallest)
        {
            smallest = cpu(i)->tasks.length;
            cpu_id = i;
        }
    }

    vec_push(&cpu(cpu_id)->tasks, task);
    
    UNLOCK(sched);
}

bool sched_is_running(void)
{
    return sched_running;
}

task_t *sched_current(void)
{
    task_t *task = cpu_self()->tasks.data[cpu_self()->current];
    return task;
}

task_t *sched_by_ident(uint64_t ident)
{
    task_t *current;
    size_t index;

    for (size_t i = 0; i < cpu_get_count(); i++)
    {
        vec_foreach(&cpu(i)->tasks, current, index)
        {
            if (current->ident == ident)
            {
                return current;
            }
        }
    }

    return NULL;
}

task_t *sched_by_pid(pid_t pid)
{
    task_t *current;
    size_t index;

    for (size_t i = 0; i < cpu_get_count(); i++)
    {
        vec_foreach(&cpu(i)->tasks, current, index)
        {
            if (current->pid == pid)
            {
                return current;
            }
        }
    }

    return NULL;
}
