cmake_minimum_required(VERSION 3.10)

project(Vee LANGUAGES CXX)

file(GLOB_RECURSE VEE "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/lib/imgui/*.hpp")
file(GLOB_RECURSE IMGUI "${CMAKE_CURRENT_LIST_DIR}/lib/imgui/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/lib/imgui/*.h")

add_executable(${PROJECT_NAME} ${VEE} ${IMGUI})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/lib")

target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/dxc/lib")
target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/GLFW")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3.lib d3d12.lib dxcompiler.lib dxgi.lib)
add_subdirectory(lib/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

set(DLL_FILES
    "${CMAKE_CURRENT_LIST_DIR}/lib/dxc/bin/dxcompiler.dll"
    "${CMAKE_CURRENT_LIST_DIR}/lib/dxc/bin/dxil.dll"
    "${CMAKE_CURRENT_LIST_DIR}/lib/d3d12/lib/D3D12Core.dll"
    "${CMAKE_CURRENT_LIST_DIR}/lib/d3d12/lib/D3D12Core.pdb"
    "${CMAKE_CURRENT_LIST_DIR}/lib/d3d12/lib/d3d12SDKLayers.dll"
    "${CMAKE_CURRENT_LIST_DIR}/lib/d3d12/lib/d3d12SDKLayers.pdb"
)

foreach(DLL_FILE ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_FILE} $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying ${DLL_FILE} to the output directory"
    )
endforeach()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:VEE_DEBUG>
        $<$<CONFIG:Release>:VEE_RELEASE>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_LIST_DIR}/bin/x64/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/x64/Release"
)