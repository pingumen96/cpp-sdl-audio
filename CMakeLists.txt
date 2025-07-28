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

# Try to find nlohmann_json, with fallback for system packages
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(NLOHMANN_JSON nlohmann_json)
        if(NLOHMANN_JSON_FOUND)
            add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
            target_include_directories(nlohmann_json::nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIRS})
        else()
            # Manual fallback for header-only installation
            if(EXISTS "/usr/local/include/nlohmann/json.hpp")
                add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
                target_include_directories(nlohmann_json::nlohmann_json INTERFACE /usr/local/include)
                set(nlohmann_json_FOUND TRUE)
            endif()
        endif()
    endif()
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


add_executable(sdl_app
src/main.cpp
src/AudioSample.cpp
src/game/Game.h
src/game/input/SceneInputSystem.cpp
src/game/Game.cpp
src/ecs/ComponentManager.cpp
src/core/GLContext.cpp
src/core/Renderer.cpp
src/core/OpenGLBackend.cpp
src/core/Window.cpp
# Resource Management System
src/resources/IFileSystem.cpp
src/resources/LoaderFactory.cpp
src/resources/ResourceRegistry.cpp
src/resources/ResourceManager.cpp
src/resources/types/TextureResource.cpp
src/resources/types/MeshResource.cpp
src/resources/loaders/TextureLoaders.cpp
src/resources/loaders/MeshLoaders.cpp
# Scene Management System
src/scene/Scene.cpp
src/scene/Scene2D.cpp
src/scene/SceneManager.cpp
# Renderer2D System (needed for Color::White and other constants)
src/scene/rendering/Renderer2D.cpp
src/game/player_state/StandingState.cpp
src/game/player_state/DuckingState.cpp
src/game/player_state/AirborneState.cpp
)

target_include_directories(sdl_app
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
)

string(REPLACE "\\" "/" PROJECT_SRC_ESC "${CMAKE_SOURCE_DIR}")
target_compile_definitions(sdl_app PRIVATE
PROJECT_SOURCE_DIR=\"${PROJECT_SRC_ESC}\"
)

add_custom_command(TARGET sdl_app POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
"${CMAKE_SOURCE_DIR}/data"
"$<TARGET_FILE_DIR:sdl_app>/data")

# Platform-specific settings
if(WIN32)
    set_target_properties(sdl_app PROPERTIES
        LINK_FLAGS "-mconsole"
    )
    # Windows-specific SDL2 linking (static libraries from vcpkg)
    target_link_libraries(sdl_app PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image-static
        SDL2_mixer::SDL2_mixer-static
        SDL2_ttf::SDL2_ttf-static
        OpenGL::GL
        GLEW::GLEW
    )
else()
    # Linux-specific SDL2 linking (shared libraries)
    target_link_libraries(sdl_app PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
        GLEW::GLEW
    )
endif()

# Link nlohmann_json if found
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(sdl_app PRIVATE nlohmann_json::nlohmann_json)
endif()

# ECS Demo executable
add_executable(ecs_demo
    src/ecs_demo.cpp
    src/ecs/ComponentManager.cpp
    src/scene/ComponentTypeRegistry.cpp
)

target_include_directories(ecs_demo
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ECS is header-only, so no additional libraries needed for basic demo

# Renderer2D Demo executable
add_executable(renderer2d_demo
    src/renderer2d_demo.cpp
    # Renderer2D System
    src/scene/rendering/Renderer2D.cpp
    # Scene Management System
    src/scene/Scene.cpp
    src/scene/SceneManager.cpp
    # Resource Management System
    src/resources/IFileSystem.cpp
    src/resources/LoaderFactory.cpp
    src/resources/ResourceRegistry.cpp
    src/resources/ResourceManager.cpp
    src/resources/types/TextureResource.cpp
    src/resources/types/MeshResource.cpp
    src/resources/loaders/TextureLoaders.cpp
    src/resources/loaders/MeshLoaders.cpp
)

target_include_directories(renderer2d_demo
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link required libraries for renderer2d demo
if(WIN32)
    target_link_libraries(renderer2d_demo PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image-static
        SDL2_mixer::SDL2_mixer-static
        SDL2_ttf::SDL2_ttf-static
        OpenGL::GL
        GLEW::GLEW
    )
else()
    target_link_libraries(renderer2d_demo PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
        GLEW::GLEW
    )
endif()

# Link nlohmann_json if found
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(renderer2d_demo PRIVATE nlohmann_json::nlohmann_json)
endif()

# Scene System Demo executable
add_executable(scene_demo
    src/scene_demo.cpp
    # ECS System
    src/ecs/ComponentManager.cpp
    src/scene/ComponentTypeRegistry.cpp
    # Scene Management System
    src/scene/Scene.cpp
    src/scene/SceneManager.cpp
    # Resource Management System
    src/resources/IFileSystem.cpp
    src/resources/LoaderFactory.cpp
    src/resources/ResourceRegistry.cpp
    src/resources/ResourceManager.cpp
    src/resources/types/TextureResource.cpp
    src/resources/types/MeshResource.cpp
    src/resources/loaders/TextureLoaders.cpp
    src/resources/loaders/MeshLoaders.cpp
    # Renderer2D System (Color::White, ecc)
    src/scene/rendering/Renderer2D.cpp
)

target_include_directories(scene_demo
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link required libraries for scene demo
if(WIN32)
    target_link_libraries(scene_demo PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image-static
        SDL2_mixer::SDL2_mixer-static
        SDL2_ttf::SDL2_ttf-static
        OpenGL::GL
        GLEW::GLEW
    )
else()
    target_link_libraries(scene_demo PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
        GLEW::GLEW
    )
endif()

# Link nlohmann_json if found
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(scene_demo PRIVATE nlohmann_json::nlohmann_json)
endif()


add_executable(sdl_appTests
src/tests/main_test.cpp
src/tests/resource_system_test.cpp
src/tests/filesystem_normalize_test.cpp
src/tests/ecs_test.cpp
src/tests/math_library_test.cpp
src/tests/renderer2d_test.cpp
src/tests/scene/scene_system_test.cpp
src/tests/scene/simple_test.cpp
src/tests/scene/minimal_test.cpp
src/tests/scene/debug_components.cpp
src/tests/scene/registry_debug.cpp
src/tests/scene/scene_debug.cpp
src/tests/scene/onattach_debug.cpp
src/tests/scene/simple_scene_test.cpp
# src/tests/scene/onattach_detail_debug.cpp  # Temporarily disabled due to resource loading
src/tests/scene/scene_integration_test.cpp
src/tests/resources/mock_resource_test.cpp
# src/tests/scene/resource_debug.cpp  # Temporarily disabled due to OpenGL dependency
# ECS System (for tests)
src/ecs/ComponentManager.cpp
src/scene/ComponentTypeRegistry.cpp
# Renderer2D System (for tests)
src/scene/rendering/Renderer2D.cpp
# Resource Management System (for tests)
src/resources/IFileSystem.cpp
src/resources/LoaderFactory.cpp
src/resources/ResourceRegistry.cpp
src/resources/ResourceManager.cpp
src/resources/types/TextureResource.cpp
src/resources/types/MeshResource.cpp
src/resources/loaders/TextureLoaders.cpp
src/resources/loaders/MeshLoaders.cpp
# Scene Management System (for tests)
src/scene/Scene.cpp
src/scene/SceneManager.cpp
)

target_link_libraries(sdl_appTests PRIVATE Catch2::Catch2WithMain)

# Add the same libraries as the main app for resource system tests
if(WIN32)
    target_link_libraries(sdl_appTests PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image-static
        SDL2_mixer::SDL2_mixer-static
        SDL2_ttf::SDL2_ttf-static
        OpenGL::GL
        GLEW::GLEW
    )
else()
    target_link_libraries(sdl_appTests PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
        GLEW::GLEW
    )
endif()

add_test(sdl_appTests
  NAME sdl_appTests
  COMMAND sdl_appTests
)

target_include_directories(sdl_appTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(CTest)
include(Catch)
catch_discover_tests(sdl_appTests)