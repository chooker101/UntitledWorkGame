#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Command/halcyonic_command_pool.hpp>
#include <DrawInfo/halcyonic_draw_info.hpp>
#include <DrawInfo/halcyonic_draw_buffer.hpp>

using namespace hal;

hal::DrawBuffer::DrawBuffer(const CommandPool* commandPool, const std::vector<const DrawInfo*>& drawInfo): mCommandPool(commandPool)
{
	vDrawInfos.resize(drawInfo.size());
	for (uint32_t i = 0; i < drawInfo.size(); ++i)
	{
		vDrawInfos[i] = drawInfo[i];
	}
	mCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mCommandBufferAllocateInfo.commandPool = mCommandPool->GetVKCommandPool();
	mCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mCommandBufferAllocateInfo.commandBufferCount = 1;

	HALCYONIC_VK_CHECK(vkAllocateCommandBuffers(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mCommandBufferAllocateInfo, &mCommandBuffer), "DrawBuffer: Could not allocate command buffer");
}

const VkCommandBuffer & hal::DrawBuffer::GetCommandBuffer() const
{
	return mCommandBuffer;
}

void hal::DrawBuffer::StartDrawBuffer()
{
	mCommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	mCommandBufferInfo.pNext = nullptr;

	HALCYONIC_VK_CHECK(vkBeginCommandBuffer(mCommandBuffer, &mCommandBufferInfo), "DrawBuffer: Could not start command buffer");
}

void hal::DrawBuffer::EndDrawBuffer()
{
	HALCYONIC_VK_CHECK(vkEndCommandBuffer(mCommandBuffer), "DrawBuffer: Could not end command buffer");
}

