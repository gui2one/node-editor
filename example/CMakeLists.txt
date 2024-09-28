cmake_minimum_required(VERSION 3.20)

project(node_editor_example)

add_executable(${PROJECT_NAME} 
    example.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE node_editor)