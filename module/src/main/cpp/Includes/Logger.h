#ifndef DAWN_LOGGER_H
#define DAWN_LOGGER_H

#include <android/log.h>

#define TAG "ModMenu"

// OBFUSCATE স্ট্রিংগুলোকে ফরম্যাট এরর ছাড়া প্রিন্ট করার জন্য %s যুক্ত করা হলো
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__))

#endif
