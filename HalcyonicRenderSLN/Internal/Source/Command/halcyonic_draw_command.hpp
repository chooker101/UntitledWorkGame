#pragma once

namespace hal
{
	enum class BufferType;
	class BufferPlaceholder //Move to shared base and header. Maybe templatize for other lookups?
	{
		friend class DrawCommand;
		BufferType mBufferType;
		uint32_t mIndex;
		std::function<const VkBuffer*(BufferType, uint32_t)> mFunction;
		BufferPlaceholder(BufferType type, uint32_t index, std::function<const VkBuffer*(BufferType, uint32_t)> function) : mBufferType(type), mIndex(index), mFunction(function) {}
	public:
		operator const VkBuffer*() const;
		operator VkBuffer() const;
	};

	class BufferLengthPlaceholder
	{
		friend class DrawCommand;
		BufferType mBufferType;
		uint32_t mIndex;
		std::function<uint32_t(BufferType, uint32_t)> mFunction;
		BufferLengthPlaceholder(BufferType type, uint32_t index, std::function<uint32_t(BufferType, uint32_t)> function) : mBufferType(type), mIndex(index), mFunction(function) {}
	public:
		operator uint32_t() const;
	};

	class DrawInfo;
	class DrawCommand
	{
	private:
		friend class DrawBuffer;
		static const DrawInfo* mCurrentDrawInfo;
		static const VkBuffer* GetBuffer(BufferType bufferType, uint32_t index);
		static uint32_t GetBufferLength(BufferType bufferType, uint32_t index);
	public:
		static BufferPlaceholder GetBufferPlaceholder(BufferType bufferType, uint32_t index);
		static BufferLengthPlaceholder GetBufferLengthPlaceholder(BufferType bufferType, uint32_t index);
	};
}