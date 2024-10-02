cmake_minimum_required(VERSION 3.20)

project(node_editor_example)


# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
add_executable(${PROJECT_NAME} 
    example.cpp
    StringGenerator.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC /W3)
target_link_libraries(${PROJECT_NAME} PUBLIC node_editor)