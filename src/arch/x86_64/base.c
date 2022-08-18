#include <kernel/arch.h>
#include <kernel/logging.h>
#include <kernel/pmm.h>
#include <kernel/loader.h>

#include "acpi.h"
#include "madt.h"
#include "gdt.h"
#include "idt.h"
#include "vmm.h"
#include "syscall.h"
#include "pit.h"
#include "pic.h"

void arch_init(void)
{
    gdt_init();
    idt_init();
    pmm_init();
    vmm_init();
    acpi_init();
    pit_init();
    pic_init();
    pic_mask_interrupts();
    pic_disable();
    apic_init();
    syscall_init();
    intstack_init();

    loader_boot_other_cpus();
    cpu_enable_int();

    __asm__ volatile ("sti");
}
