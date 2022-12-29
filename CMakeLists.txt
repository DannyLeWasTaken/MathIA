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

add_executable(MathIA src/main.cpp)


target_link_libraries(${PROJECT_NAME} PRIVATE glm)