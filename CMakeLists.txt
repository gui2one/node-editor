cmake_minimum_required(VERSION 3.20)

include(FetchContent)   

set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NODE_EDITOR_BUILD_EXAMPLE "Build node editor example" ON)

project(node_editor)

function(FetchDependencyAndMakeAvailable NAME URL TAG)
  FetchContent_Declare(
    ${NAME}
    GIT_REPOSITORY ${URL}
    GIT_TAG        ${TAG}
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(${NAME})
endfunction()

set(imgui_sources 
  # imgui stuff
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/backends/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/backends/imgui_impl_opengl3.cpp
  
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/misc/cpp/imgui_stdlib.cpp

  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/imgui.cpp
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/imgui_demo.cpp
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/imgui_tables.cpp     
)

add_library(imgui STATIC ${imgui_sources})
# target_compile_options(imgui PRIVATE -W1)

target_include_directories(imgui PUBLIC 
  ${CMAKE_SOURCE_DIR}/_deps/imgui-src
  ${CMAKE_SOURCE_DIR}/_deps/glfw-src/include
)




add_library(${PROJECT_NAME} STATIC

  src/yaml_serialize.cpp
  src/NodeFactory.cpp
  src/ImGuiNode.cpp
  src/NodeParam.cpp
  src/NodeManager.cpp
  src/Event.cpp
  src/EventDispatcher.cpp
  src/EventManager.cpp
  src/utils.cpp
  src/utils/node_manager_utils.cpp
)


# add_library(serialize_lib STATIC 
#  src/yaml_serialize.cpp
# )
# target_include_directories(serialize_lib PUBLIC 
#   src/
#   ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src
#   ${CMAKE_SOURCE_DIR}/build/_deps/yaml-cpp-src/include
#   ${_include_dirs}
# )

# target_link_libraries(serialize_lib PUBLIC yaml-cpp::yaml-cpp)
# target_link_libraries(serialize_lib PUBLIC imgui)


target_compile_options(${PROJECT_NAME} PUBLIC /W3)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
# target_link_libraries(${PROJECT_NAME} PUBLIC serialize_lib)



set(_include_dirs
  src/
  vendor/glad/include
  ${CMAKE_SOURCE_DIR}/build/_deps/glfw-src/include
  ${CMAKE_SOURCE_DIR}/build/_deps/glm-src/
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui-src/backends
  ${CMAKE_SOURCE_DIR}/build/_deps/imgui_node_editor-src
)


# YAML-CPP
set(YAML_CPP_BUILD_TOOLS OFF)
set(BUILD_SHARED_LIBS OFF)
FetchDependencyAndMakeAvailable(yaml-cpp https://github.com/jbeder/yaml-cpp da82fd982c260e7f335ce5acbceff24b270544d1) #master 10/11/2024

target_include_directories(${PROJECT_NAME} PUBLIC build/_deps/yaml-cpp-src/include)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp::yaml-cpp)


# GLM
FetchDependencyAndMakeAvailable(
  glm
  https://github.com/g-truc/glm
  master
)

# GLFW
set(GLFW_BUILD_SHARED_LIBRARY ON)
FetchDependencyAndMakeAvailable(
  glfw
  https://github.com/glfw/glfw
  7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)

target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE build/_deps/glfw-src/include)

# GLAD
add_subdirectory(vendor/glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)


#IMGUI
FetchDependencyAndMakeAvailable(
  imgui
  https://github.com/ocornut/imgui
  v1.91.2-docking
)


target_include_directories(${PROJECT_NAME} PUBLIC 
  ${_include_dirs}
)


include_directories(
  
  ${_include_dirs}
)



if(${NODE_EDITOR_BUILD_EXAMPLE})
  add_subdirectory(example)
endif()
