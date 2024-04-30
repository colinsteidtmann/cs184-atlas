cmake_minimum_required(VERSION 3.13)  # CMake version check
project(Atlas)               # create project "Atlas"


option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(lib/glfw)



option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(lib/assimp)



if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

set(CMAKE_CXX_STANDARD 14)            # enable c++14 standard

# Set the path to the resources directory
set(RESOURCES_DIR "${PROJECT_SOURCE_DIR}/resources")

# add source files
set(SOURCE_FILES lib/glad.c src/main.cpp src/loader.cpp src/waterRenderer.cpp src/grassRenderer.cpp src/snowyGrassRenderer.cpp lib/stb_image.cpp lib/stb_image.hpp src/waterFrameBuffer.cpp)

# add executable target with source files listed in SOURCE_FILES variable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_definitions(-DRESOURCES_DIR="${RESOURCES_DIR}")

# find packages
find_package(OpenGL REQUIRED)

# link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC assimp glfw
        ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} ${OPENGL_LIBRARIES})

# include directories
include_directories(lib/assimp/include/
                    lib/glad/include/
                    lib/glfw/include/
                    lib/glm/
                    lib/stb/
                    ${RESOURCES_DIR})
file(GLOB VENDORS_SOURCES lib/glad/src/glad.c)
source_group("Vendors" FILES ${VENDORS_SOURCES})


target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

# Copy the resources directory to the build directory
file(COPY "${RESOURCES_DIR}"
     DESTINATION "${CMAKE_BINARY_DIR}/resources"
     FOLLOW_SYMLINK_CHAIN)