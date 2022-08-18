#include "arch.h"
#include "loader.h"
#include "pmm.h"
#include "logging.h"
#include "elf.h"
#include "sched.h"

#include <assert.h>
#include <stddef.h>

int _start(void)
{
    arch_init();
    sched_init();


    for (;;);

    __builtin_unreachable();
}
