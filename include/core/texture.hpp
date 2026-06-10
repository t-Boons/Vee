#pragma once

#include "common.hpp"

namespace vee
{
    struct TextureProperties
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t NumChannels;
        void* Data = nullptr;
        std::string DebugName = "Texture";
    };

	class Texture : public NonCopyable
    {
    public:
        Texture(const TextureProperties& properties) : m_properties(properties) {}
        virtual ~Texture() = default;

    protected:
        TextureProperties m_properties;
    };

    struct TextureCubeProperties
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t NumChannels;
        std::array<void*, 6> Data = {};
        std::string DebugName = "TextureCube";
    };


    class TextureCube : public NonCopyable
    {
    public:
        TextureCube(const TextureCubeProperties& properties) : m_properties(properties) {}
        virtual ~TextureCube() = default;

    protected:
        TextureCubeProperties m_properties;
    };
}
