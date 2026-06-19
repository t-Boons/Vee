#include "core/sky.hpp"
#include "tinygltf/stb_image.h"

#include "platform/vulkan/vulkan_texture.hpp"

namespace vee
{
	void Sky::SetupSkybox(const SkyProperties& properties)
	{
		TextureCubeProperties cubeProperties{};

		std::array<int, CUBEMAP_FACE_COUNT> textureWidths;
		std::array<int, CUBEMAP_FACE_COUNT> textureHeights;
		std::array<int, CUBEMAP_FACE_COUNT> textureNumChannels;

		stbi_set_flip_vertically_on_load(1);

		for (size_t i = 0; i < CUBEMAP_FACE_COUNT; i++)
		{
			cubeProperties.Data[i] = stbi_load(properties.TextureFilePaths[i].c_str(), &textureWidths[i], &textureHeights[i], &textureNumChannels[i], 4);
			CheckMsg(cubeProperties.Data[i] != nullptr, "Failed to load skybox texture: %s", properties.TextureFilePaths[i].c_str());
		}


		for (size_t i = 0; i < CUBEMAP_FACE_COUNT; i++)
		{
			if (textureWidths[i] != textureWidths[0] || textureHeights[i] != textureHeights[0] || textureNumChannels[i] != textureNumChannels[0])
			{
				CheckMsg(false, "All skybox textures must have the same dimensions and number of channels.");
			}
		}
		

		cubeProperties.Width = textureWidths[0];
		cubeProperties.Height = textureHeights[0];
		cubeProperties.NumChannels = textureNumChannels[0];
		cubeProperties.DebugName = "Skybox";
		m_texture = TextureCube::Create(cubeProperties);



		if (!m_buffersInitialized)
		{
			m_buffersInitialized = true;

			float vertices[] = {
				// +X
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f, -1.0f,
				// -X
			  -1.0f, -1.0f,  1.0f,
			  -1.0f, -1.0f, -1.0f,
			  -1.0f,  1.0f, -1.0f,
			  -1.0f,  1.0f,  1.0f,
			  // +Y
			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			// -Y
		  -1.0f, -1.0f,  1.0f,
		   1.0f, -1.0f,  1.0f,
		   1.0f, -1.0f, -1.0f,
		  -1.0f, -1.0f, -1.0f,
		  // +Z
		 -1.0f, -1.0f,  1.0f,
		 -1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		  // -Z
		  1.0f, -1.0f, -1.0f,
		  1.0f,  1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,
			};

			uint32_t indices[] = {
				 0,  1,  2,  2,  3,  0,
				 4,  5,  6,  6,  7,  4,
				 8,  9, 10, 10, 11,  8,
				12, 13, 14, 14, 15, 12,
				16, 17, 18, 18, 19, 16,
				20, 21, 22, 22, 23, 20,
			};


			BufferProperties vertexBufferProperties{};
			vertexBufferProperties.Usage = BufferUsage::Vertex;
			vertexBufferProperties.MemoryType = MemoryType::Static;
			vertexBufferProperties.Size = sizeof(vertices);
			vertexBufferProperties.Data = vertices;
			vertexBufferProperties.DebugName = "SkyboxVertices";
			m_vertexBuffer = Buffer::Create(vertexBufferProperties);

			BufferProperties indexBufferProperties{};
			indexBufferProperties.Usage = BufferUsage::Index;
			indexBufferProperties.MemoryType = MemoryType::Static;
			indexBufferProperties.Size = sizeof(indices);
			indexBufferProperties.Data = indices;
			indexBufferProperties.DebugName = "SkyboxIndices";
			m_indexBuffer = Buffer::Create(indexBufferProperties);
		}
	}
}
