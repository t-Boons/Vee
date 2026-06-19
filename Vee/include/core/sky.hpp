#pragma once

#include "common.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include <glm/glm.hpp>

namespace vee
{
	struct SkyProperties
	{
		std::array<std::string, CUBEMAP_FACE_COUNT> TextureFilePaths;
	};

	class Sky
	{
	public:
		void SetupSkybox(const SkyProperties& properties);

		RefPtr<TextureCube> m_texture;
		RefPtr<Buffer> m_vertexBuffer;
		RefPtr<Buffer> m_indexBuffer;
		bool m_buffersInitialized = false;
	};
}