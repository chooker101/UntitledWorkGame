#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Buffer/halcyonic_buffer.hpp>

using namespace hal;

void hal::Buffer::CreateBuffer(const VkBufferCreateInfo& bufferCreateInfo)
{
	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	HALCYONIC_VK_CHECK(vmaCreateBuffer(Render::Instance()->GetAllocator(), &bufferCreateInfo, &allocCreateInfo, &mBuffer, &mAllocation, &mAllocationInfo), "Buffer: Could not create Vma Buffer");
}

hal::Buffer::Buffer(uint32_t size, uint8_t * pData, BufferType bufferType) : mBufferSize(size), mBufferType(bufferType)
{
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(bufferType);

	CreateBuffer(bufferCreateInfo);
	UpdateBuffer(pData);
}

hal::Buffer::Buffer(uint32_t size, uint8_t * pData, BufferType bufferType, VkSharingMode sharingMode)
{
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(bufferType);
	bufferCreateInfo.sharingMode = sharingMode;

	CreateBuffer(bufferCreateInfo);
	UpdateBuffer(pData);
}

void hal::Buffer::UpdateBuffer(uint8_t * pData)
{
	void* mappedData = nullptr;
	HALCYONIC_VK_CHECK(vmaMapMemory(Render::Instance()->GetAllocator(), mAllocation, &mappedData), "Buffer: Could not map Vma memory");
	memcpy(mappedData, pData, mBufferSize);
	vmaUnmapMemory(Render::Instance()->GetAllocator(), mAllocation);
}

hal::Buffer::~Buffer()
{
	vmaDestroyBuffer(Render::Instance()->GetAllocator(), mBuffer, mAllocation);
}