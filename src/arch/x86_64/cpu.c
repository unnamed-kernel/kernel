
#include <kernel/arch.h>
#include <kernel/klock.h>
#include <kernel/logging.h>
#include <kernel/utils.h>

#include "cpu.h"
#include "madt.h"
#include "gdt.h"
#include "idt.h"
#include "syscall.h"
#include "vmm.h"

DECLARE_LOCK(smp);

extern void *calloc(size_t, size_t);

static cpu_t *cpus = NULL;
static size_t cpus_count = 0;
static size_t cpus_ready = 1;

void init_cpus(size_t count)
{
    cpus_count = count;

    cpus = calloc(count, sizeof(cpu_t));
    if (cpus == NULL)
    {
        klog(ERROR, "Could not allocate memory for cpus");
        halt();
    }
}

void cpu_enable_int(void)
{
    for (size_t i = 0; i < cpus_count; i++)
    {
        cpu(i)->int_enable = true;
    }
}

size_t cpu_get_count(void)
{
    return cpus_count;
}

size_t cpu_get_ready(void)
{
    return cpus_ready;
}

cpu_t *cpu(int id)
{
    return &cpus[id];
}

cpu_t *cpu_self(void)
{
    return &cpus[lapic_current_cpu()];
}

void _start_cpus(void)
{
    if (cpus == NULL)
    {
        klog(ERROR, "CPUs not initialized");
        halt();
    }

    LOCK(smp);

    klog(INFO, "Booting up...", lapic_current_cpu());

    gdt_init();
    idt_init();

    vmm_switch_space(vmm_get_kernel_pml());

    syscall_init();
    intstack_init();
    lapic_init();

    __asm__ volatile("cli");

    lapic_timer_init();

    vec_init(&cpu_self()->tasks);

    cpus_ready++;

    UNLOCK(smp);

    while (!cpu_self()->int_enable)
    {
        __asm__ volatile ("pause");
    }

    __asm__ volatile("sti");

    for (;;);
}
