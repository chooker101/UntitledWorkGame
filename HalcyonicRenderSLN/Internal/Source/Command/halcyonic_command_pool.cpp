#include<precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include<Command/halcyonic_command_pool.hpp>

hal::CommandPool::CommandPool(uint32_t queueFamilyIndex)
{
	mCommandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mCommandPoolCI.queueFamilyIndex = queueFamilyIndex;
	mCommandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	HALCYONIC_VK_CHECK(vkCreateCommandPool(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mCommandPoolCI, nullptr, &mCommandPool), "CommandPool: Could not create command pool");
}
