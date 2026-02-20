#include "ksu_mgr.h"

#include <errno.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#define KSU_INSTALL_MAGIC1 0xDEADBEEF
#define KSU_INSTALL_MAGIC2 0xCAFEBABE
#define KSU_IOCTL_GET_MANAGERS _IOC(_IOC_READ | _IOC_WRITE, 'K', 105, 0)

struct ksu_manager_entry {
    __u32 uid;
    __u8 signature_index;
} __attribute__((packed));

struct ksu_get_managers_cmd {
    __u16 count;
    __u16 total_count;
    struct ksu_manager_entry managers[];
} __attribute__((packed));

#define ksuctl(op, ...) (ioctl(fd, op, __VA_ARGS__) >= 0)

int ksu_get_manager_appids(uid_t **appids, size_t *count) {
    if (!appids || !count)
        return -EINVAL;
    *appids = NULL;
    *count = 0;

    int fd = -1;
    syscall(SYS_reboot, KSU_INSTALL_MAGIC1, KSU_INSTALL_MAGIC2, 0, &fd);
    if (fd < 0)
        return -errno;

    struct ksu_get_managers_cmd probe = { .count = 0 };
    if (!ksuctl(KSU_IOCTL_GET_MANAGERS, &probe)) {
        int err = errno;
        close(fd);
        return -err;
    }

    if (probe.total_count == 0) {
        close(fd);
        return 0;
    }

    size_t payload_size = sizeof(struct ksu_manager_entry) * probe.total_count;
    size_t total_size = sizeof(struct ksu_get_managers_cmd) + payload_size;
    struct ksu_get_managers_cmd *cmd = malloc(total_size);
    if (!cmd) {
        close(fd);
        return -ENOMEM;
    }

    cmd->count = probe.total_count;
    if (!ksuctl(KSU_IOCTL_GET_MANAGERS, cmd)) {
        int err = errno;
        free(cmd);
        close(fd);
        return -err;
    }

    size_t total_count = cmd->total_count;
    uid_t *arr = malloc(sizeof(uid_t) * total_count);
    if (!arr) {
        free(cmd);
        close(fd);
        return -ENOMEM;
    }

    for (size_t i = 0; i < total_count; ++i) {
        uid_t appid = cmd->managers[i].uid % PER_USER_RANGE;
        arr[i] = appid;
    }

    free(cmd);
    close(fd);

    *appids = arr;
    *count = total_count;

    return 0;
}

void ksu_free_manager_appids(uid_t *appids) {
    free(appids);
}
