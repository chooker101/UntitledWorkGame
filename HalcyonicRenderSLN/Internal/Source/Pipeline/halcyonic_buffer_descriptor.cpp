#include <precompiled.hpp>
#include <Pipeline/halcyonic_descriptor_layout.hpp>
#include <Buffer/halcyonic_buffer.hpp>
#include <Pipeline/halcyonic_buffer_descriptor.hpp>

using namespace hal;

hal::BufferDescriptor::BufferDescriptor(const DescriptorLayout* descriptorLayout, const Buffer * buffer) : Descriptor(descriptorLayout), mBuffer(buffer)
{
	mDescriptorBufferInfo.buffer = *mBuffer->GetVkBuffer();
	mDescriptorBufferInfo.offset = 0;
	mDescriptorBufferInfo.range = static_cast<VkDeviceSize>(mBuffer->GetBufferSize());
}
