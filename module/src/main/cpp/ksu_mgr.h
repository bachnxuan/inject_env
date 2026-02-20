#pragma once
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PER_USER_RANGE 100000

int ksu_get_manager_appids(uid_t **appids, size_t *count);
void ksu_free_manager_appids(uid_t *appids);

#ifdef __cplusplus
}
#endif
