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
    };

	class Texture : public NonCopyable
    {
    public:
        Texture(const TextureProperties& properties) : m_properties(properties) {}
        virtual ~Texture() = default;

    protected:
        TextureProperties m_properties;
    };
}
