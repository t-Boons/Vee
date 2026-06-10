#pragma once

#include "common.hpp"
#include "device.hpp"

namespace vee
{
    enum class BufferUsage
    {
        Vertex,
        Index,
        Uniform,
        TransferSrc,
        TransferDst
    };

    enum class MemoryType
    {
        Static,
        Dynamic
    };


    struct BufferProperties
    {
        uint32_t Size;
        void* Data = nullptr;
        BufferUsage Usage;
        MemoryType MemoryType;
		std::string DebugName = "Buffer";
    };

	class Buffer : public NonCopyable
	{
	public:
        static RefPtr<Buffer> Create(const BufferProperties& properties);

		virtual ~Buffer() = default;

		virtual void* Map() = 0;
		virtual void UnMap() = 0;
	};
}
