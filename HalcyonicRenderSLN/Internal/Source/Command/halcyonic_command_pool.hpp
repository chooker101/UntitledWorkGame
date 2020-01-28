#pragma once

namespace hal
{
	class CommandPool
	{
	private:
		VkCommandPool mCommandPool;
		VkCommandPoolCreateInfo mCommandPoolCI = {}; //pull to layout
	public:
		CommandPool(uint32_t queueFamilyIndex);
		const VkCommandPool& GetVKCommandPool() const { return mCommandPool; }
	};
}