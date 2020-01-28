#pragma once

namespace hal
{
	class CommandPool;
	class SetupCommandBuffer
	{
	private:
		const CommandPool* mCommandPool;
		VkCommandBuffer mCommandBuffer;
		VkCommandBufferAllocateInfo mCommandBufferAllocateInfo = {}; //Move to shared layout
		VkCommandBufferBeginInfo mCommandBufferInfo = {};
	public:
		SetupCommandBuffer(const CommandPool* commandPool);

		const VkCommandBuffer& GetVulkanCommandBuffer() const { return mCommandBuffer; }

		void StartSetupBuffer();
		void EndAndSubmitSetupBuffer();
		~SetupCommandBuffer();
	};
}