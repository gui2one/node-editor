cmake_minimum_required(VERSION 3.20)


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


# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
add_executable(${PROJECT_NAME} 
    src/example.cpp
    # src/utils.cpp
    src/Application.cpp
    # src/Serialize.cpp
    # src/Deserialize.cpp
    src/StringGenerator.cpp
)

include_directories(
  src/
)
# target_precompile_headers(${PROJECT_NAME} PUBLIC ../src/node_editor.h)
target_compile_options(${PROJECT_NAME} PUBLIC /W3)
target_link_libraries(${PROJECT_NAME} PUBLIC node_editor)

# find_package(Python3 REQUIRED)

# add_custom_command(
#     TARGET ${PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E echo "Running post-build Python script..."
#     COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../python/copy_ini_file_if_needed.py "$<IF:$<CONFIG:Debug>,Debug,Release>"
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#     # ARGS -E copy "${INI_FILE}" ${${CMAKE_SOURCE_DIR}}/build/example/Release/imgui.ini
# )

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
