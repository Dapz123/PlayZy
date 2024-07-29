#include "module.h"
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <thread>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "zygisk.hpp"
#include "data.h"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        const char *dataDir = env->GetStringUTFChars(args->app_data_dir, nullptr);
        preSpecialize(process, dataDir);
        env->ReleaseStringUTFChars(args->nice_name, process);
        env->ReleaseStringUTFChars(args->app_data_dir, dataDir);
    }
    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (startModule) {
            std::thread hackThread(moduleMain, appDataDir, data, length);
            hackThread.detach();
        }
    }

private:
    Api *api;
    JNIEnv *env;
    bool startModule = false;
    char *appDataDir;
    void *data;
    size_t length;

    void preSpecialize(const char *process, const char *dataDir) {
        if (strcmp(process, packageName)==0) {
            startModule = true;
            appDataDir = new char[strlen(dataDir)+1];
            strcpy(appDataDir, dataDir);
            #if defined(__i386__)
            auto path = "zygisk/armeabi-v7a.so";
            #endif
            #if defined(__x86_64__)
            auto path = "zygisk/arm64-v8a.so";
            #endif
            #if defined(__i386__) || defined(__x86_64__)
            int dirfd = api->getModuleDir();
            int fd = openat(dirfd, path, O_RDONLY);
            if (fd != -1) {
                struct stat sb{};
                fstat(fd, &sb);
                length = sb.st_size;
                data = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
                close(fd);
            }
            #endif
        } else {
          startModule = false;
          api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

};

// Register our module class
REGISTER_ZYGISK_MODULE(MyModule)
