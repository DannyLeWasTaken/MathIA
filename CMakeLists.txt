cmake_minimum_required(VERSION 3.24)
project(MathIA)
set(CMAKE_CXX_STANDARD 17)

# includes
include(FetchContent)

link_directories(${CMAKE_SOURCE_DIR}/lib)

# Add GLM as a library
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG v1.0.6)
FetchContent_MakeAvailable(tinyobjloader)

add_executable(MathIA
        src/main.cpp
        src/ray.hpp
        src/ray.cpp
        )

target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE tinyobjloader)