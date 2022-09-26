#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include "system.h"

void spinlock_lock(uint32_t *lock);
void spinlock_unlock(uint32_t *lock);

#endif