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

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)


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



target_link_libraries(sdl_app PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf)


# test
add_executable(sdl_appTests src/tests/test_vec.cpp src/tests/test_rect.cpp)

# Linka con Catch2 (versione 3.x)
target_link_libraries(sdl_appTests PRIVATE Catch2::Catch2WithMain)

add_test(sdl_appTests
  NAME sdl_appTests
  COMMAND sdl_appTests
)

# Se hai bisogno degli include di progetto
target_include_directories(sdl_appTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(CTest)
include(Catch)
catch_discover_tests(sdl_appTests)