cmake_minimum_required(VERSION 3.22.1)
project("zygiskmodmenu")

# ImGui এর সব সোর্স ফাইল খুঁজে বের করা
file(GLOB IMGUI_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/backends/imgui_impl_android.cpp
)

add_library(zygiskmodmenu SHARED
    main.cpp
    ${IMGUI_SOURCES}
)

# এখানে ImGui এর প্যারেন্ট ফোল্ডারটি (cpp ফোল্ডার) যুক্ত করা হলো, যাতে imgui/imgui.h কাজ করে
target_include_directories(zygiskmodmenu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/..
)

target_link_libraries(zygiskmodmenu
    log
    android
    dl
    GLESv2
)
