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
typedef void* (*il2cpp_class_get_method_from_name_t)(void* klass, const char* name, int argsCount);

// ফাংশন পয়েন্টার
il2cpp_domain_get_t il2cpp_domain_get;
il2cpp_domain_get_assemblies_t il2cpp_domain_get_assemblies;
il2cpp_assembly_get_image_t il2cpp_assembly_get_image;
il2cpp_class_from_name_t il2cpp_class_from_name;
il2cpp_class_get_field_from_name_t il2cpp_class_get_field_from_name;
il2cpp_field_static_get_value_t il2cpp_field_static_get_value;

// আপনার dump.cs থেকে অফসেট
#define OFFSET_SALLENTITIES 0x8

// PlayerData অফসেট
#define OFFSET_NICKNAME 0x38
#define OFFSET_ISDEAD 0x6C
#define OFFSET_LASTPOSITION 0xDC
#define OFFSET_PLAYER_PTR 0x100
#define OFFSET_TEAM_ID 0x35
#define OFFSET_LTRANSFORM_POSITION 0x64
#define OFFSET_LTRANSFORM 0x60

// মেমরি রিড হেল্পার
template<typename T>
T ReadMemory(uintptr_t address) {
    if (address == 0) return T{};
    return *(T*)address;
}

// IL2CPP API লোড করার ফাংশন
void InitIL2CPP() {
    void* handle = dlopen(targetLibName, RTLD_LAZY);
    if (!handle) return;

    il2cpp_domain_get = (il2cpp_domain_get_t)dlsym(handle, "il2cpp_domain_get");
    il2cpp_domain_get_assemblies = (il2cpp_domain_get_assemblies_t)dlsym(handle, "il2cpp_domain_get_assemblies");
    il2cpp_assembly_get_image = (il2cpp_assembly_get_image_t)dlsym(handle, "il2cpp_assembly_get_image");
    il2cpp_class_from_name = (il2cpp_class_from_name_t)dlsym(handle, "il2cpp_class_from_name");
    il2cpp_class_get_field_from_name = (il2cpp_class_get_field_from_name_t)dlsym(handle, "il2cpp_class_get_field_from_name");
    il2cpp_field_static_get_value = (il2cpp_field_static_get_value_t)dlsym(handle, "il2cpp_field_static_get_value");
}

// শত্রুদের তালিকা বের করার ফাংশন
void UpdateEntities() {
    if (!il2cpp_domain_get) return;

    void* domain = il2cpp_domain_get();
    size_t assemblyCount = 0;
    void** assemblies = il2cpp_domain_get_assemblies(domain, &assemblyCount);
    if (!assemblies) return;

    void* image = nullptr;
    for (size_t i = 0; i < assemblyCount; i++) {
        image = il2cpp_assembly_get_image(assemblies[i]);
        void* baseGameClass = il2cpp_class_from_name(image, OBFUSCATE("GCommon"), OBFUSCATE("BaseGame"));
        if (baseGameClass) {
            // আমরা BaseGame ক্লাস পেয়েছি
            void* sAllEntitiesField = il2cpp_class_get_field_from_name(baseGameClass, OBFUSCATE("sAllEntities"));
            if (sAllEntitiesField) {
                void* allEntitiesDict = nullptr;
                il2cpp_field_static_get_value(sAllEntitiesField, &allEntitiesDict);
                
                if (allEntitiesDict) {
                    // এখানে Dictionary থেকে প্লেয়ার লুপ করার কোড থাকবে
                    // Unity Dictionary Structure: _entries (0x18), _count (0x20)
                    int count = ReadMemory<int>((uintptr_t)allEntitiesDict + 0x20);
                    uintptr_t entries = ReadMemory<uintptr_t>((uintptr_t)allEntitiesDict + 0x18);
                    
                    g_EnemyCount = count; // আপাতত কাউন্ট দেখাচ্ছি
                    
                    // এখানে ইমগুই দিয়ে লাইন আঁকার লজিক বসবে
                }
            }
            break;
        }
    }
}

// মেনু ড্র করার ফাংশন
void drawMenu() {
    ImGui::Begin(OBFUSCATE("Sable ESP"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox(OBFUSCATE("ESP Lines"), &g_ShowESP);
    ImGui::Checkbox(OBFUSCATE("Enemy Count"), &g_ShowEnemyCount);
    if (g_ShowEnemyCount) {
        ImGui::Text(OBFUSCATE("Enemies: %d"), g_EnemyCount);
    }
    ImGui::End();

    if (g_ShowESP || g_ShowEnemyCount) {
        UpdateEntities();
    }
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

    LOGI(OBFUSCATE("Done"));
    return nullptr;
}

#endif // MODMENU_H
