#pragma once

#include "common.hpp"
#include "device.hpp"

#define CUBEMAP_FACE_COUNT 6

namespace vee
{
    struct TextureProperties
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t NumChannels;
        void* Data = nullptr;
        std::string DebugName = "Unnamed";
    };

	class Texture : public NonCopyable
    {
    public:
        static RefPtr<Texture> Create(const TextureProperties& properties);

        virtual ~Texture() = default;
    };

    struct TextureCubeProperties
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t NumChannels;
        std::array<void*, CUBEMAP_FACE_COUNT> Data = {};
        std::string DebugName = "Unnamed";
    };


    class TextureCube : public NonCopyable
    {
    public:
        static RefPtr<TextureCube> Create(const TextureCubeProperties& properties);

        virtual ~TextureCube() = default;
    };
}
