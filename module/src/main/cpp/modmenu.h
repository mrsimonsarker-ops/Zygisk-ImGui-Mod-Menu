//
// Created by Reveny on 2023/1/13.
//
#define targetLibName OBFUSCATE("libil2cpp.so")

#include <jni.h>
#include <sys/system_properties.h>
#include <vector>
#include <cmath>

#include "KittyMemory/MemoryPatch.h"
#include "Includes/ESP.h"
#include "Includes/Dobby/dobby.h"
#include "Includes/Utils.h"
#include "Includes/ImGui.h"

// আমাদের গ্লোবাল ভেরিয়েবল
static int enable_hack;
static char *game_data_dir = nullptr;
static char *game_package_name = "com.dts.freefiremax";

static bool g_ShowESP = false;
static bool g_ShowEnemyCount = false;
static int g_EnemyCount = 0;

// আপনার dump.cs থেকে পাওয়া অফসেটসমূহ
#define OFFSET_GAMEENGINE_INSTANCE 0x0
#define OFFSET_CURRENTGAME 0x20
#define OFFSET_SALLENTITIES 0x8

// PlayerData অফসেট
#define OFFSET_NICKNAME 0x38
#define OFFSET_ISDEAD 0x6C
#define OFFSET_LASTPOSITION 0xDC
#define OFFSET_PLAYER_PTR 0x100
#define OFFSET_TEAM_ID 0x35

// Player Class LTransform
#define OFFSET_LTRANSFORM 0x60
#define OFFSET_LTRANSFORM_POSITION 0x64

// Unity Camera ফাংশনের জন্য
typedef void* (*Camera_get_main_t)();
typedef void (*WorldToScreenPoint_t)(void* camera, void* pos, void* out);

Camera_get_main_t Camera_get_main;
WorldToScreenPoint_t WorldToScreenPoint;

// মেমরি রিড টেমপ্লেট
template<typename T>
T ReadMemory(uintptr_t address) {
    if (address == 0) return T{};
    return *(T*)address;
}

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
    if (strcmp(package_name, game_package_name) == 0) {
        game_data_dir = new char[strlen(app_data_dir) + 1];
        strcpy(game_data_dir, app_data_dir);
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 1;
    } else {
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 0;
    }
}

void drawMenu() {
    ImGui::Begin(OBFUSCATE("Sable ESP"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::Checkbox(OBFUSCATE("ESP Lines"), &g_ShowESP);
    ImGui::Checkbox(OBFUSCATE("Enemy Count"), &g_ShowEnemyCount);
    
    if (g_ShowEnemyCount) {
        ImGui::Text(OBFUSCATE("Enemies: %d"), g_EnemyCount);
    }
    
    ImGui::End();

    // এখানে আমরা গেমের ভেতরের শত্রুদের খুঁজবো
    if (g_ShowESP || g_ShowEnemyCount) {
        if (!isLibraryLoaded(targetLibName)) return;

        uintptr_t baseAddr = getBaseAddress(targetLibName);
        if (!baseAddr) return;

        // GameEngine.instance বের করা (এটি একটি Static Field, যা সরাসরি অ্যাড্রেস দিয়ে রিড করা যায়)
        uintptr_t gameEngineInstance = ReadMemory<uintptr_t>(baseAddr + OFFSET_GAMEENGINE_INSTANCE); // *সতর্কতা*: 0x0 অফসেট সাধারণত সঠিক নয়, এটি একটি উদাহরণ
        
        // যেহেতু Static Field রিড করা কঠিন, আপাতত আমরা এই অংশটি কমেন্ট করে রাখছি।
        // পরবর্তীতে IL2CPP API যুক্ত করে এটি ঠিক করতে হবে।
    }
}

void *hack_thread(void *) {
    LOGI(OBFUSCATE("hack thread: %d"), gettid());
    initModMenu((void *)drawMenu);

    //Check if target lib is loaded
    do {
        LOGI(OBFUSCATE("Trying to find lib... PID: %d"), getpid());
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    LOGI(OBFUSCATE("%s has been loaded, 0x%lx"), (const char *) targetLibName, getBaseAddress(targetLibName));

    // Camera ফাংশনগুলো রেজলভ করা (RVA ব্যবহার করে)
    // Camera.get_main = (Camera_get_main_t)getAbsoluteAddress(targetLibName, 0xa0ce614);
    // WorldToScreenPoint = (WorldToScreenPoint_t)getAbsoluteAddress(targetLibName, 0xa0ce298);

    LOGI(OBFUSCATE("Done"));
    return nullptr;
}
