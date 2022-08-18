#include <kernel/pmm.h>
#include <kernel/logging.h>
#include <kernel/utils.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "task.h"

extern void *calloc(size_t, size_t);

task_t *task_create(void *space, char const *path, uintptr_t ip)
{
    task_t *self = calloc(sizeof(task_t), 1);
    self->space = space;
    self->stack = pmm_alloc(STACK_SIZE);
    
    memcpy(self->path, path, UNIX_PATH_LIMIT);

    assert(self->stack != NULL);

    vmm_map(space, (virtual_physical_map_t) {
        .physical = (uintptr_t) self->stack,
        .virtual = USER_STACK_BASE,
        .length = STACK_SIZE
    }, true);

    self->context = context_create(ip);
    return self;
}
