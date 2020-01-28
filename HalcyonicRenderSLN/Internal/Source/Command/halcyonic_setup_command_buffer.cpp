#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Command/halcyonic_command_pool.hpp>
#include <Command/halcyonic_setup_command_buffer.hpp>

hal::SetupCommandBuffer::SetupCommandBuffer(const CommandPool * commandPool): mCommandPool(commandPool)
{
	mCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mCommandBufferAllocateInfo.commandPool = mCommandPool->GetVKCommandPool();
	mCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mCommandBufferAllocateInfo.commandBufferCount = 1;

	HALCYONIC_VK_CHECK(vkAllocateCommandBuffers(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mCommandBufferAllocateInfo, &mCommandBuffer), "SetupCommandBuffer: Could not allocate command buffer");

	mCommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
}

void hal::SetupCommandBuffer::StartSetupBuffer()
{
	vkBeginCommandBuffer(mCommandBuffer, &mCommandBufferInfo);
}

void hal::SetupCommandBuffer::EndAndSubmitSetupBuffer()
{
	vkEndCommandBuffer(mCommandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffer;

	vkQueueSubmit(hal::Render::Instance()->GetVulkanQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(hal::Render::Instance()->GetVulkanQueue());
}

hal::SetupCommandBuffer::~SetupCommandBuffer()
{
	vkFreeCommandBuffers(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mCommandPool->GetVKCommandPool(), 1, &mCommandBuffer);
	mCommandBuffer = VK_NULL_HANDLE;
}
