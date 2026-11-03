#ifndef DAWN_LOGGER_H
#define DAWN_LOGGER_H

#include <android/log.h>

#define TAG "ModMenu"

// OBFUSCATE স্ট্রিংগুলোকে ফরম্যাট এরর ছাড়া প্রিন্ট করার জন্য নতুন ম্যাক্রো
#define LOGD(fmt, ...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, "%s", (const char*)(fmt)))
#define LOGE(fmt, ...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, "%s", (const char*)(fmt)))
#define LOGI(fmt, ...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, "%s", (const char*)(fmt)))
#define LOGW(fmt, ...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, "%s", (const char*)(fmt)))

#endif
