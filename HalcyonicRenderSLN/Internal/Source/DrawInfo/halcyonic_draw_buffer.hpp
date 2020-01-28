#pragma once
#include<Command/halcyonic_draw_command.hpp>

namespace hal
{
	class DrawInfo;
	class CommandPool;

	class DrawBuffer
	{
	private:
		const CommandPool* mCommandPool;
		VkCommandBuffer mCommandBuffer;
		VkCommandBufferAllocateInfo mCommandBufferAllocateInfo; //Move to own layout
		VkCommandBufferBeginInfo mCommandBufferInfo;
		std::vector<const DrawInfo*> vDrawInfos;
	public:
		DrawBuffer() = default;
		DrawBuffer(const CommandPool* commandPool, const std::vector<const DrawInfo*>& drawInfo);

		const VkCommandBuffer& GetCommandBuffer() const;

		void StartDrawBuffer();
		
		//Sends command to all DrawInfos
		template<typename TFPTR, typename ...ARGS>
		void RecordVulkanCommands(TFPTR&& vkCmd, ARGS&& ...args); 

		//Send command to specific DrawInfo
		template<typename TFPTR, typename ...ARGS>
		void RecordSingleVulkanCommand(uint32_t index, TFPTR&& vkCmd, ARGS&& ...args);

		void EndDrawBuffer();
		~DrawBuffer() = default;
	};
#include <DrawInfo/halcyonic_draw_buffer.inl>
}