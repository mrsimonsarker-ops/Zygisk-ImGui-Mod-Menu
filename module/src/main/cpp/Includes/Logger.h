#ifndef DAWN_LOGGER_H
#define DAWN_LOGGER_H

#include <android/log.h>
#include "obfuscate.h" // OBFUSCATE ম্যাক্রোর জন্য

//Change this to another Log Tag if ya want.
#define TAG "ModMenu"

// স্ট্যান্ডার্ড অ্যান্ড্রয়েড লগ লেভেল ব্যবহার করা হলো, এবং এরর এড়াতে %s যুক্ত করা হলো
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__))

#endif //DAWN_LOGGER_H
