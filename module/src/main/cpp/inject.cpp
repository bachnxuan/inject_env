/* Copyright (c) 2026 bachnxuan
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "ksu_mgr.h"
#include "zygisk.hpp"
#include <algorithm>
#include <android/log.h>
#include <cstring>
#include <errno.h>
#include <stdlib.h>
#include <vector>

using zygisk::Api;
using zygisk::AppSpecializeArgs;

const char *MODULE_NAME = "InjectEnv";

#define LOGD(...) \
    __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGI(...) \
    __android_log_print(ANDROID_LOG_INFO, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) \
    __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)

class InjectEnv : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        const uid_t uid = static_cast<uid_t>(args->uid);
        const uid_t appid = uid % PER_USER_RANGE;

        if (is_manager(appid, mgr_appid_cache())) {
            int ret = setenv("ZYGISK_ENABLED", "1", 1);
            if (ret != 0) {
                LOGE("setenv failed: %d", errno);
            } else {
                LOGD("setenv success for appid=%u", appid);
            }
        }
    }

private:
    Api *api;
    JNIEnv *env;

    static std::vector<uid_t> build_mgr_appid_cache() {
        auto v = get_mgr_appids();
        return std::vector<uid_t>(v.begin(), v.end());
    }

    static const std::vector<uid_t> &mgr_appid_cache() {
        static const std::vector<uid_t> cache = build_mgr_appid_cache();
        return cache;
    }

    static std::vector<uid_t> get_mgr_appids() {
        uid_t *appids = nullptr;
        size_t count{};

        int ret = ksu_get_manager_appids(&appids, &count);
        if (ret != 0) {
            LOGE("ksu_get_manager_appids failed: %d", ret);
            return {};
        }

        std::vector<uid_t> v(appids, appids + count);
        ksu_free_manager_appids(appids);

        return v;
    }

    static bool is_manager(uid_t appid, const std::vector<uid_t> &v) {
        return std::find(v.begin(), v.end(), appid) != v.end();
    }
};

REGISTER_ZYGISK_MODULE(InjectEnv)