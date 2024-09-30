cmake_minimum_required(VERSION 3.20)

project(node_editor_example)

add_executable(${PROJECT_NAME} 
    example.cpp
    StringGenerator.cpp
)
target_compile_options(${PROJECT_NAME} PUBLIC /W4 /external:W0)
target_link_libraries(${PROJECT_NAME} PUBLIC node_editor)