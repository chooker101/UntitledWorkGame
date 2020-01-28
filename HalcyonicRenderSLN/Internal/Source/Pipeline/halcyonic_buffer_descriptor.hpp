#pragma once
#include <Pipeline/halcyonic_descriptor.hpp>

namespace hal
{
	class Buffer;
	class BufferDescriptor : public Descriptor
	{
	private:
		const Buffer* mBuffer;
		VkDescriptorBufferInfo mDescriptorBufferInfo = {};
	public:
		BufferDescriptor(const DescriptorLayout* descriptorLayout, const Buffer* buffer);
		const VkDescriptorBufferInfo& GetDescriptorBufferInfo() const { return mDescriptorBufferInfo; }
	};
}