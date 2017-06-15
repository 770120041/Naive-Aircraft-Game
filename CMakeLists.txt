cmake_minimum_required(VERSION 3.7)
project(MegaProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework GLUT -framework OpenGL -lGLEW -lglfw")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp libs/myGL.cpp libs/myGL.h components/Aircraft/Aircraft.cpp components/Aircraft/Aircraft.h libs/myGLHeaders.h components/SkyBox/SkyBox.cpp components/SkyBox/SkyBox.h components/Floor/Floor.cpp components/Floor/Floor.h libs/PerlinNoise.cpp libs/PerlinNoise.h components/Terrain/Terrain.cpp components/Terrain/Terrain.h components/AABB/AABB.cpp components/AABB/AABB.h)

include_directories(
        glm
        /usr/local/Cellar/glew/2.0.0/include/
        /usr/local/Cellar/glfw/3.2.1/include/
        /opt/local/include/SOIL/
)

link_directories(
        /usr/local/Cellar/glew/2.0.0/lib/
        /usr/local/Cellar/glfw/3.2.1/lib/
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(MegaProject ${SOURCE_FILES})

target_link_libraries(MegaProject /opt/local/lib/libSOIL.dylib)
