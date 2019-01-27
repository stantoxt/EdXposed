//
// Created by Solo on 2019/1/27.
//

#include <cstdio>
#include <unistd.h>
#include <jni.h>
#include <cstdlib>
#include <array>
#include <thread>
#include <vector>
#include <string>
#include <include/logging.h>
#include "config_manager.h"

#define BLACK_LIST_PATH "/data/misc/riru/modules/edxposed/blacklist/"
#define WHITE_LIST_PATH "/data/misc/riru/modules/edxposed/whitelist/"
#define USE_WHITE_LIST "/data/misc/riru/modules/edxposed/usewhitelist"
#define GLOBAL_MODE "/data/misc/riru/modules/edxposed/forceglobal"

static char package_name[256];

// default is true
int is_app_need_hook(JNIEnv *env, jstring appDataDir) {
    if (is_global_mode()) {
        return 1;
    }
    if (!appDataDir) {
        LOGW("appDataDir is null");
        return 1;
    }
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    int user = 0;
    if (sscanf(app_data_dir, "/data/%*[^/]/%d/%s", &user, package_name) != 2) {
        if (sscanf(app_data_dir, "/data/%*[^/]/%s", package_name) != 1) {
            package_name[0] = '\0';
            LOGW("can't parse %s", app_data_dir);
            return 1;
        }
    }
    env->ReleaseStringUTFChars(appDataDir, app_data_dir);
    bool use_white_list = access(USE_WHITE_LIST, F_OK) == 0;
    bool white_list_exists = access(WHITE_LIST_PATH, F_OK) == 0;
    bool black_list_exists = access(BLACK_LIST_PATH, F_OK) == 0;
    if (use_white_list && white_list_exists) {
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, WHITE_LIST_PATH "%s", package_name);
        int res = access(path, F_OK) == 0;
        LOGD("use whitelist, res=%d", res);
        return res;
    } else if (!use_white_list && black_list_exists) {
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, BLACK_LIST_PATH "%s", package_name);
        int res = access(path, F_OK) != 0;
        LOGD("use blacklist, res=%d", res);
        return res;
    } else {
        LOGD("use nonlist, res=%d", 1);
        return 1;
    }
}

int is_global_mode() {
    return access(GLOBAL_MODE, F_OK) == 0;
}
