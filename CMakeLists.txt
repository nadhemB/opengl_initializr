cmake_minimum_required(VERSION 3.10)
project(OpenGLInitializr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Force static link for GLFW
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_SHARED_LIBS OFF)

# Add submodules
add_subdirectory(extern/glfw)
add_subdirectory(extern/glad)

# Include directories
include_directories(
    extern/glad/include
    extern/glfw/include
    extern/glm
    include
)

# OpenGL core is always needed
find_package(OpenGL REQUIRED)

# Executable
add_executable(opengl_initializer main.cpp)

# Link everything statically
target_link_libraries(OpenGLProject
    glfw
    glad
    ${CMAKE_DL_LIBS}
    OpenGL::GL
)

# Windows-specific static OpenGL
if(WIN32)
    target_link_libraries(OpenGLProject opengl32)
endif()
