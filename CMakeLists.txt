cmake_minimum_required(VERSION 3.20)

include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

option(NODE_EDITOR_BUILD_EXAMPLE "Build node editor example" ON)
option(NODE_EDITOR_BUILD_DOC "Build node editor documentation" OFF)

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

# Define the source directory
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Recursively collect all .cpp and .h files
file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"
)
# Organize files into folders in Visual Studio
foreach(FILE_PATH IN LISTS SOURCES)
    # Get the directory relative to the source directory
    file(RELATIVE_PATH REL_PATH "${SOURCE_DIR}" "${FILE_PATH}")
    
    # Extract the directory part of the relative path
    get_filename_component(FOLDER_PATH "${REL_PATH}" DIRECTORY)

    # Replace "/" with "\\" to ensure compatibility with Visual Studio
    string(REPLACE "/" "\\" FOLDER_PATH "src/${FOLDER_PATH}")

    # Assign the file to a source group
    source_group("${FOLDER_PATH}" FILES "${FILE_PATH}")
endforeach()

add_library(${PROJECT_NAME} STATIC
  
    ${SOURCES}
    # src/yaml_serialize.cpp
  # src/NodeFactory.cpp
  # src/ParamFactory.cpp
  # src/NodeParam.cpp
  # src/ImGuiNode.cpp
  # src/NodeManager.cpp
  # src/Event.cpp
  # src/EventDispatcher.cpp
  # src/EventManager.cpp
  # src/utils.cpp
  # src/utils/node_manager_utils.cpp
  # src/stb_image.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC /W4)
target_include_directories(${PROJECT_NAME} PUBLIC ${_include_dirs})
include_directories(${_include_dirs})

target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)

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

