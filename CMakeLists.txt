cmake_minimum_required(VERSION 3.10)

project(Vee LANGUAGES CXX)

file(GLOB_RECURSE VEE "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/lib/imgui/*.hpp")
file(GLOB_RECURSE IMGUI "${CMAKE_CURRENT_LIST_DIR}/lib/imgui/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/lib/imgui/*.h")

add_executable(${PROJECT_NAME} ${VEE} ${IMGUI})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/lib")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/lib/Vulkan/include")
target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/GLFW")

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3.lib)

add_subdirectory(lib/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)


target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:VEE_DEBUG>
        $<$<CONFIG:Release>:VEE_RELEASE>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_LIST_DIR}/bin/x64/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/x64/Release"
)