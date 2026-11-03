#ifndef MODMENU_H
#define MODMENU_H

#include <jni.h>
#include <dlfcn.h>
#include <cstring>
#include <cmath>
#include <android/log.h>
#include "Includes/obfuscate.h"
#include "Includes/Logger.h"
#include "Includes/Utils.h"
#include "Includes/ImGui.h"

#define targetLibName OBFUSCATE("libil2cpp.so")

// গ্লোবাল ভেরিয়েবল
static bool g_ShowESP = false;
static bool g_ShowEnemyCount = false;
static int g_EnemyCount = 0;

// IL2CPP API টাইপ ডেফিনিশন
typedef void* (*il2cpp_domain_get_t)();
typedef void** (*il2cpp_domain_get_assemblies_t)(void* domain, size_t* size);
typedef void* (*il2cpp_assembly_get_image_t)(void* assembly);
typedef void* (*il2cpp_class_from_name_t)(void* image, const char* namespaze, const char* name);
typedef void* (*il2cpp_class_get_field_from_name_t)(void* klass, const char* name);
typedef void (*il2cpp_field_static_get_value_t)(void* field, void* value);

// ফাংশন পয়েন্টার
il2cpp_domain_get_t il2cpp_domain_get = nullptr;
il2cpp_domain_get_assemblies_t il2cpp_domain_get_assemblies = nullptr;
il2cpp_assembly_get_image_t il2cpp_assembly_get_image = nullptr;
il2cpp_class_from_name_t il2cpp_class_from_name = nullptr;
il2cpp_class_get_field_from_name_t il2cpp_class_get_field_from_name = nullptr;
il2cpp_field_static_get_value_t il2cpp_field_static_get_value = nullptr;

// Camera ফাংশন পয়েন্টার
void* (*Camera_get_main)() = nullptr;
void (*WorldToScreenPoint)(void* camera, void* pos, void* out) = nullptr;

// PlayerData অফসেট (আপনার dump.cs থেকে)
#define OFFSET_ISDEAD 0x6C
#define OFFSET_LASTPOSITION 0xDC
#define OFFSET_TEAM_ID 0x35

// মেমরি রিড হেল্পার
template<typename T>
T ReadMemory(uintptr_t address) {
    if (address == 0) return T{};
    return *(T*)address;
}

// IL2CPP API লোড
void InitIL2CPP() {
    void* handle = dlopen(targetLibName, RTLD_LAZY);
    if (!handle) return;

    il2cpp_domain_get = (il2cpp_domain_get_t)dlsym(handle, "il2cpp_domain_get");
    il2cpp_domain_get_assemblies = (il2cpp_domain_get_assemblies_t)dlsym(handle, "il2cpp_domain_get_assemblies");
    il2cpp_assembly_get_image = (il2cpp_assembly_get_image_t)dlsym(handle, "il2cpp_assembly_get_image");
    il2cpp_class_from_name = (il2cpp_class_from_name_t)dlsym(handle, "il2cpp_class_from_name");
    il2cpp_class_get_field_from_name = (il2cpp_class_get_field_from_name_t)dlsym(handle, "il2cpp_class_get_field_from_name");
    il2cpp_field_static_get_value = (il2cpp_field_static_get_value_t)dlsym(handle, "il2cpp_field_static_get_value");
    
    // Camera ফাংশন সেটআপ (RVA ব্যবহার করে)
    uintptr_t baseAddr = getBaseAddress(targetLibName);
    if (baseAddr) {
        Camera_get_main = (void*(*)())(baseAddr + 0xa0ce614);
        WorldToScreenPoint = (void(*)(void*, void*, void*))(baseAddr + 0xa0ce298);
    }
}

// ESP লাইন আঁকা
void DrawESP() {
    if (!il2cpp_domain_get || !Camera_get_main) return;

    void* camera = Camera_get_main();
    if (!camera) return;

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    
    // লাইনের শুরুর বিন্দু: স্ক্রিনের একদম উপরের মাঝখানে (X=মাঝখান, Y=0)
    ImVec2 startPos = ImVec2(screenSize.x / 2.0f, 0.0f);

    void* domain = il2cpp_domain_get();
    size_t assemblyCount = 0;
    void** assemblies = il2cpp_domain_get_assemblies(domain, &assemblyCount);
    if (!assemblies) return;

    for (size_t i = 0; i < assemblyCount; i++) {
        void* image = il2cpp_assembly_get_image(assemblies[i]);
        void* baseGameClass = il2cpp_class_from_name(image, OBFUSCATE("GCommon"), OBFUSCATE("BaseGame"));
        if (baseGameClass) {
            void* sAllEntitiesField = il2cpp_class_get_field_from_name(baseGameClass, OBFUSCATE("sAllEntities"));
            if (sAllEntitiesField) {
                void* allEntitiesDict = nullptr;
                il2cpp_field_static_get_value(sAllEntitiesField, &allEntitiesDict);
                
                if (allEntitiesDict) {
                    // ডিকশনারি থেকে শত্রু লুপ
                    int count = ReadMemory<int>((uintptr_t)allEntitiesDict + 0x20);
                    uintptr_t entries = ReadMemory<uintptr_t>((uintptr_t)allEntitiesDict + 0x18);
                    
                    g_EnemyCount = 0;

                    for (int j = 0; j < count; j++) {
                        uintptr_t entry = entries + (j * 0x20);
                        void* playerData = ReadMemory<void*>((uintptr_t)entry + 0x10);
                        
                        if (!playerData) continue;

                        bool isDead = ReadMemory<bool>((uintptr_t)playerData + OFFSET_ISDEAD);
                        if (isDead) continue;

                        g_EnemyCount++;

                        // শত্রুর পজিশন বের করা
                        float enemyPos[3];
                        memcpy(enemyPos, (void*)((uintptr_t)playerData + OFFSET_LASTPOSITION), sizeof(enemyPos));

                        // WorldToScreenPoint কল
                        float screenPos[3];
                        WorldToScreenPoint(camera, enemyPos, screenPos);

                        if (screenPos[2] > 0) { // ক্যামেরার সামনে থাকলে
                            float screenX = screenPos[0];
                            float screenY = screenSize.y - screenPos[1]; // Y অক্ষ সমন্বয়

                            // সাদা রঙের লাইন আঁকা (স্ক্রিনের একদম উপরের মাঝখান থেকে শত্রুর দিকে)
                            drawList->AddLine(
                                startPos,
                                ImVec2(screenX, screenY),
                                IM_COL32(255, 255, 255, 255), // একদম সাদা রঙ
                                2.0f // লাইনের পুরুত্ব
                            );
                        }
                    }
                }
            }
            break;
        }
    }
}

// মেনু ড্র
void drawMenu() {
    ImGui::Begin(OBFUSCATE("Sable ESP"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox(OBFUSCATE("ESP Lines"), &g_ShowESP);
    ImGui::Checkbox(OBFUSCATE("Enemy Count"), &g_ShowEnemyCount);
    if (g_ShowEnemyCount) {
        ImGui::Text(OBFUSCATE("Enemies: %d"), g_EnemyCount);
    }
    ImGui::End();

    if (g_ShowESP || g_ShowEnemyCount) {
        DrawESP();
    }
}

// গেম ডিটেকশন
int isGame(JNIEnv *env, jstring appDataDir) {
    if (!appDataDir) return 0;
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    static char package_name[256];
    if (sscanf(app_data_dir, OBFUSCATE("/data/%*[^/]/%d/%s"), 0, package_name) != 2) {
        if (sscanf(app_data_dir, OBFUSCATE("/data/%*[^/]/%s"), package_name) != 1) {
            package_name[0] = '\0';
            env->ReleaseStringUTFChars(appDataDir, app_data_dir);
            return 0;
        }
    }
    if (strcmp(package_name, OBFUSCATE("com.dts.freefiremax")) == 0) {
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 1;
    }
    env->ReleaseStringUTFChars(appDataDir, app_data_dir);
    return 0;
}

// হ্যাক থ্রেড
void* hack_thread(void *) {
    LOGI(OBFUSCATE("hack thread: %d"), gettid());
    initModMenu((void *)drawMenu);

    do {
        LOGI(OBFUSCATE("Trying to find lib... PID: %d"), getpid());
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    InitIL2CPP();

    LOGI("Done"));
    return nullptr;
}

#endif
