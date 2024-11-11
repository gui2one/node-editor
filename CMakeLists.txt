cmake_minimum_required(VERSION 3.20)

include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

option(NODE_EDITOR_BUILD_EXAMPLE "Build node editor example" ON)
option(NODE_EDITOR_BUILD_DOC "Build node editor documentation" ON)

project(node_editor)

set(_include_dirs
  src/
  vendor/glad/include
  vendor/
  ${CMAKE_SOURCE_DIR}/build/_deps/glfw-src/include
  ${CMAKE_SOURCE_DIR}/build/_deps/glm-src/
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/backends
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui_node_editor-src
)

add_library(${PROJECT_NAME} STATIC
  src/yaml_serialize.cpp
  src/NodeFactory.cpp
  src/ParamFactory.cpp
  src/NodeParam.cpp
  src/ImGuiNode.cpp
  src/NodeManager.cpp
  src/Event.cpp
  src/EventDispatcher.cpp
  src/EventManager.cpp
  src/utils.cpp
  src/utils/node_manager_utils.cpp
  src/stb_image.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${_include_dirs})
include_directories(${_include_dirs})

target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)
target_compile_options(${PROJECT_NAME} PUBLIC /W3)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)

target_include_directories(${PROJECT_NAME} PUBLIC build/_deps/yaml-cpp-src/include)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp::yaml-cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE build/_deps/glfw-src/include)

# GLAD
add_subdirectory(vendor/glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)

if(NODE_EDITOR_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

if(NODE_EDITOR_BUILD_DOC)
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Building Documentation ..."
  COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} build_docs.bat
  COMMAND ${CMAKE_COMMAND} -E echo "Documentation build complete"
)
endif()

