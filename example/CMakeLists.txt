cmake_minimum_required(VERSION 3.20)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(FetchDependencyAndMakeAvailable NAME URL TAG)
  FetchContent_Declare(
    ${NAME}
    GIT_REPOSITORY ${URL}
    GIT_TAG        ${TAG}
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(${NAME})
endfunction()


project(node_editor_example)

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
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    # src/example.cpp
    # src/Application.cpp
    # src/StringGenerator.cpp
    # src/NumberOperator.cpp
)

include_directories(
  src/
)

# target_compile_options(${PROJECT_NAME} PUBLIC /W3)
target_link_libraries(${PROJECT_NAME} PUBLIC node_editor)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_CURRENT_BINARY_DIR}$<IF:$<CONFIG:Debug>,/Debug,/Release>/resources"
)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/example_resources" "${CMAKE_CURRENT_BINARY_DIR}$<IF:$<CONFIG:Debug>,/Debug,/Release>/example_resources"
)


# # YAML-CPP
# set(YAML_CPP_BUILD_TOOLS OFF)
# FetchDependencyAndMakeAvailable(yaml-cpp https://github.com/jbeder/yaml-cpp da82fd982c260e7f335ce5acbceff24b270544d1) #master 10/11/2024
# target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp::yaml-cpp)
# target_include_directories(${PROJECT_NAME} PUBLIC build/_deps/yaml-cpp-src/include)
