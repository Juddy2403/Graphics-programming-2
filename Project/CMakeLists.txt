# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${SHADER_SOURCE_DIR}/*.frag"
        "${SHADER_SOURCE_DIR}/*.vert"
)

foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)


set(SOURCES
        "main.cpp"
        "vulkanbase/VulkanBase.h"
        "vulkanbase/VulkanUtil.h"
        # Add other source files here
        "vulkanbase/VulkanBase.cpp"

        "Shader.h" "Shader.cpp" "CommandPool.h" "CommandPool.cpp" "CommandBuffer.h" "CommandBuffer.cpp" "meshes/3DMesh.h" "meshes/3DMesh.cpp"
        "Level.h" "Level.cpp" "RenderPass.h" "RenderPass.cpp"
        "GraphicsPipeline.h" "GraphicsPipeline.cpp" "Camera.h" "TimeManager.h" "Singleton.h" "DescriptorPool.h" "DescriptorPool.cpp"
        "DepthBuffer.h" "DepthBuffer.cpp" "DataBuffer.cpp" "DataBuffer.h" "meshes/Vertex.h" "meshes/2DMesh.cpp" "meshes/2DMesh.h"
        SwapChain.cpp
        SwapChain.h
        texture/ImageView.cpp
        texture/ImageView.h
        texture/Texture.cpp
        texture/Texture.h
        meshes/Transform.cpp
        meshes/Transform.h
        texture/TextureLoader.cpp
        texture/TextureLoader.h
        meshes/MeshLoader.cpp
        meshes/MeshLoader.h
        LevelParser.h
        LevelParser.cpp
        texture/TextureManager.cpp
        texture/TextureManager.h
)
#
# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES} "vulkanbase/VulkanUtil.cpp")
add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})