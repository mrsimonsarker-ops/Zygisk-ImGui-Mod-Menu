cmake_minimum_required(VERSION 3.22.1)
project("zygiskmodmenu")

add_library(zygiskmodmenu SHARED
    main.cpp
    modmenu.h
)

target_link_libraries(zygiskmodmenu
    log
    android
    dl
)

# ImGui এবং অন্যান্য ফোল্ডারের হেডার ফাইলগুলো যুক্ত করা
target_include_directories(zygiskmodmenu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes
)
