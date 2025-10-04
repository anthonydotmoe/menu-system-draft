cmake_minimum_required(VERSION 3.16)
project(MenuSystemDraft LANGUAGES C CXX)

# --- Options

# --- Standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

# --- Compile options
add_library(project_warnings INTERFACE)
if(MSVC)
    target_compile_options(project_warnings INTERFACE /W4)
else()
    target_compile_options(project_warnings INTERFACE
        -Wall -Wextra -Wpedantic
        -ffunction-sections -fdata-sections
    )
endif()

add_library(link_gc INTERFACE)
if(APPLE)
    target_link_options(link_gc INTERFACE -Wl,-dead_strip)
else()
    target_link_options(link_gc INTERFACE -Wl,--gc-sections)
endif()

# --- Target

add_executable(menu_system_draft
    src/main.c
    src/ui/ui_core.c
    src/ui/ui_editors.c
    src/modules/mod_aprs.c
)
target_link_libraries(menu_system_draft PRIVATE project_warnings link_gc)
target_include_directories(menu_system_draft
    PRIVATE
    include
)