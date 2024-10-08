cmake_minimum_required(VERSION 3.20)

project(node_editor_example)


# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
add_executable(${PROJECT_NAME} 
    example.cpp
    StringGenerator.cpp
)

# target_precompile_headers(${PROJECT_NAME} PUBLIC ../src/node_editor.h)
target_compile_options(${PROJECT_NAME} PUBLIC /W3)
target_link_libraries(${PROJECT_NAME} PUBLIC node_editor)

find_package(Python3 REQUIRED)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running post-build Python script..."
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/copy_ini_file_if_needed.py "$<IF:$<CONFIG:Debug>,Debug,Release>"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    # ARGS -E copy "${INI_FILE}" ${${CMAKE_SOURCE_DIR}}/build/example/Release/imgui.ini
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_CURRENT_BINARY_DIR}$<IF:$<CONFIG:Debug>,/Debug,/Release>/resources"
)
