cmake_minimum_required(VERSION 3.21)
project(sdl_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(MSVC)
add_compile_options(/W4 /permissive- /WX)
add_compile_options(/utf-8)
set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /W4 /WX /MDd /permissive-")
else()
#add_compile_options(-Wall -Wextra -Werror)
endif()

list(APPEND CMAKE_PREFIX_PATH E:/Progetti/vcpkg/installed/x64-mingw-static/share/sdl2)

find_package(SDL2 REQUIRED)


add_executable(sdl_app
src/main.cpp
src/AudioSample.cpp
)

set_target_properties(sdl_app PROPERTIES
    LINK_FLAGS "-mconsole"
)

target_include_directories(sdl_app
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 1. Definisci PROJECT_SOURCE_DIR come macro C++ contenente la cartella root
string(REPLACE "\\" "/" PROJECT_SRC_ESC "${CMAKE_SOURCE_DIR}")
target_compile_definitions(sdl_app PRIVATE
PROJECT_SOURCE_DIR=\"${PROJECT_SRC_ESC}\"
)

# 2. Continua a fare il copy della directory data in /data accanto all’exe
add_custom_command(TARGET sdl_app POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
"${CMAKE_SOURCE_DIR}/data"
"$<TARGET_FILE_DIR:sdl_app>/data")



target_link_libraries(sdl_app PRIVATE SDL2::SDL2)