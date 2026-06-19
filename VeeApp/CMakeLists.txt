cmake_minimum_required(VERSION 3.10)

project(VeeApp LANGUAGES CXX)

add_executable(VeeApp main.cpp)
target_link_libraries(VeeApp PRIVATE Vee)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_LIST_DIR}/bin/x64/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/x64/Release"
)