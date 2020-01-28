#pragma once

namespace hal
{
	enum class BufferType
	{
		TransferBuffer = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		ConstantBuffer = UniformBuffer,
		StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		IndirectBuffer = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
	};

	class Buffer //Todo: add offset
	{
	private:
		VmaAllocation mAllocation;
		VmaAllocationInfo mAllocationInfo;
		VkBuffer mBuffer;
		uint32_t mBufferSize;
		BufferType mBufferType;

		void CreateBuffer(const VkBufferCreateInfo& bufferCreateInfo); //Maybe move more to shared area
	public:
		Buffer(uint32_t size, uint8_t* pData, BufferType bufferType);
		Buffer(uint32_t size, uint8_t* pData, BufferType bufferType, VkSharingMode sharingMode);

		BufferType GetBufferType() const { return mBufferType; }
		const VkBuffer* GetVkBuffer() const { return &mBuffer; }
		uint32_t GetBufferSize() const { return mBufferSize; }

		void UpdateBuffer(uint8_t* pData);

		~Buffer();
	};
}