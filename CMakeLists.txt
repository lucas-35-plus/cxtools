cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(CXTools VERSION 1.0.0 LANGUAGES C CXX)

set(PROJECT_NAME "CXTools")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_HEADERS_IN_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -Os -s")
endif()

# 编译源码设置
# headers
include_directories(imgui)
include_directories(include)
include_directories(App)
include_directories(image_editor)
link_directories(${PROJECT_NAME} libs)

# project source files
file(GLOB_RECURSE IMGUI_SRC imgui/*.h imgui/*.cpp)
file(GLOB_RECURSE THIRD_PARTY include/*.h include/*.cc)
file(GLOB_RECURSE APP_SRC App/*.h App/*.cc)
file(GLOB_RECURSE IMAGE_EDITOR image_editor/*.h image_editor/*.c)

# project executable build result
add_executable(${PROJECT_NAME} main.cc ${IMGUI_SRC} ${THIRD_PARTY} ${APP_SRC} ${IMAGE_EDITOR})
# add_executable(${PROJECT_NAME} glfw_main.cc ${IMGUI_SRC} ${THIRD_PARTY} ${APP_SRC} ${IMAGE_EDITOR})

add_subdirectory(libjpeg)
include_directories(libjpeg)
target_link_libraries(${PROJECT_NAME} jpeg)

# 编译Library导入
if(DEFINED MINGW32)
# 设置交叉编译的目标操作为windows

# static link glfw
add_library(glfw
  STATIC
  IMPORTED)
set_target_properties(glfw
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_SOURCE_DIR}/libs/libglfw3_win.a)
target_link_libraries(${PROJECT_NAME} glfw)

# static link avif
add_library(avif
  STATIC
  IMPORTED)
set_target_properties(avif PROPERTIES IMPORTED_LOCATION
  ${CMAKE_SOURCE_DIR}/libs/libavif_win.a)
target_link_libraries(${PROJECT_NAME} avif)

target_link_libraries(${PROJECT_NAME} -static-libstdc++ -static-libgcc -lopengl32 
-Wl,-Bstatic,--whole-archive -lwinpthread -lssp -Wl,--no-whole-archive -Wl,-Bdynamic)

else()
# 设置交叉编译Linux

# import X11 library
find_package(X11 REQUIRED)
message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

# static link glfw
add_library(glfw
  STATIC
  IMPORTED)
set_target_properties(glfw
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_SOURCE_DIR}/libs/libglfw3_linux.a)
target_link_libraries(${PROJECT_NAME} glfw)

# static link avif
add_library(avif
  STATIC
  IMPORTED)
set_target_properties(avif PROPERTIES IMPORTED_LOCATION
  ${CMAKE_SOURCE_DIR}/libs/libavif_linux.a)
target_link_libraries(${PROJECT_NAME} avif)

target_link_libraries(${PROJECT_NAME} -static-libstdc++)
target_link_libraries(${PROJECT_NAME} GL)
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})

endif()
