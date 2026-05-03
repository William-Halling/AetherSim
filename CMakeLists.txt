cmake_minimum_required(VERSION 3.28)
project(AetherSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # Helpful for VSCode/Clangd

# --- Dependencies via FetchContent ---
include(FetchContent)


FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG master
)


FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)


FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.0
)


FetchContent_MakeAvailable(entt glm spdlog)


# Main executable
add_executable(aethersim
    src/main.cpp
    # Add more .cpp files here as we create them
)


target_include_directories(aethersim PRIVATE include)
target_link_libraries(aethersim PRIVATE EnTT::EnTT glm::glm spdlog::spdlog)


# Release flags
if(MSVC)
    target_compile_options(aethersim PRIVATE /W4 /permissive- /O2)
else()
    target_compile_options(aethersim PRIVATE -Wall -Wextra -O3 -march=native)
endif()


# enable_testing()
