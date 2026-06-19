cmake_minimum_required(VERSION 3.10)

project(Vee LANGUAGES CXX)

file(GLOB_RECURSE VEE   "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
                        "${CMAKE_CURRENT_LIST_DIR}/lib/imgui/*.hpp"
                        "${CMAKE_CURRENT_LIST_DIR}/lib/tinygltf/*.h"
)

add_library(Vee STATIC ${VEE})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/lib")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/lib/Vulkan/include")

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/GLFW/glfw3.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/lib/GLFW"
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

add_subdirectory(lib/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

target_link_libraries(${PROJECT_NAME} PRIVATE GPUOpen::VulkanMemoryAllocator)
add_subdirectory(lib/VulkanMemoryAllocator)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/mikktspace)
target_link_libraries(${PROJECT_NAME} PRIVATE mikktspace)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/lib/mikktspace")

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/lib/VulkanMemoryAllocator/include")

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:VEE_DEBUG>
        $<$<CONFIG:Release>:VEE_RELEASE>
)