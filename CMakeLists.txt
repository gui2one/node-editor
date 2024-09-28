cmake_minimum_required(VERSION 3.20)

include(FetchContent)   
Set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
  build/_deps/imgui-src/backends/imgui_impl_glfw.cpp
  build/_deps/imgui-src/backends/imgui_impl_opengl3.cpp

  build/_deps/imgui-src/imgui.cpp
  build/_deps/imgui-src/imgui_demo.cpp
  build/_deps/imgui-src/imgui_draw.cpp
  build/_deps/imgui-src/imgui_widgets.cpp
  build/_deps/imgui-src/imgui_tables.cpp     
)
add_library(imgui STATIC ${imgui_sources})
target_include_directories(imgui PUBLIC 
  build/_deps/imgui-src
  build/_deps/glfw-src/include
)
add_library(${PROJECT_NAME} STATIC

  src/ImGuiNode.cpp
  src/NodeManager.cpp
 
)

target_link_libraries(${PROJECT_NAME} PUBLIC imgui)

set(_include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/glm-src/
  ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/imgui-src/
  ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/imgui-src/backends
  ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/imgui_node_editor-src
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

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
target_include_directories(${PROJECT_NAME} PRIVATE 
  ${_include_dirs}
)


include_directories(
  
  ${_include_dirs}
)

add_subdirectory(example)

