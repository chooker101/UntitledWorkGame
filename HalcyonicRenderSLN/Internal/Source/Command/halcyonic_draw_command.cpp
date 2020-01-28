#include <precompiled.hpp>
#include <Buffer/halcyonic_buffer.hpp>
#include <DrawInfo/halcyonic_draw_info.hpp>
#include <Command/halcyonic_draw_command.hpp>

using namespace hal;

const DrawInfo* DrawCommand::mCurrentDrawInfo = nullptr;

const VkBuffer* DrawCommand::GetBuffer(BufferType bufferType, uint32_t index)
{
	return mCurrentDrawInfo->GetBuffer(bufferType, index)->GetVkBuffer();
}

uint32_t DrawCommand::GetBufferLength(BufferType bufferType, uint32_t index)
{
	return mCurrentDrawInfo->GetBuffer(bufferType, index)->GetBufferSize();
}

BufferPlaceholder hal::DrawCommand::GetBufferPlaceholder(BufferType bufferType, uint32_t index)
{
	return { bufferType, index, std::bind(GetBuffer, bufferType, index) };
}

BufferLengthPlaceholder hal::DrawCommand::GetBufferLengthPlaceholder(BufferType bufferType, uint32_t index)
{
	return { bufferType, index, std::bind(GetBufferLength, bufferType, index) };
}

hal::BufferPlaceholder::operator const VkBuffer*() const
{
	return mFunction(mBufferType, mIndex);
}

hal::BufferPlaceholder::operator VkBuffer() const
{
	return *mFunction(mBufferType, mIndex);
}

hal::BufferLengthPlaceholder::operator uint32_t() const
{
	return mFunction(mBufferType, mIndex);
}