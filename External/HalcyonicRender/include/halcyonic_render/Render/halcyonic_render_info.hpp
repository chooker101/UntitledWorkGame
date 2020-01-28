#pragma once

namespace hal
{
	class DrawBuffer;
	class Semaphore;
	class RenderInfo
	{
	private:
		std::vector<DrawBuffer*> vDrawCommandBuffers;//Should be const
		std::vector<VkCommandBuffer> vRawDrawBuffers;
		Semaphore* mSemaphore = nullptr;
		VkSubmitInfo mSubmitInfo;
		VkPipelineStageFlags mWaitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	public:
		RenderInfo() = default;
		RenderInfo(std::vector<DrawBuffer*> drawBuffers);
		RenderInfo(std::vector<DrawBuffer*> drawBuffers, Semaphore* bufferSemaphore);

		uint32_t AddDrawBuffer(DrawBuffer* drawBuffers);
		void SetSemaphore(Semaphore* bufferSemaphore);

		const DrawBuffer* GetDrawBuffer(uint32_t index) const;
		const std::vector<DrawBuffer*>& GetDrawBufferList() const { return vDrawCommandBuffers; }
		const VkSubmitInfo& GetSubmitInfo() const;

		void BuildRenderinfo();
		void BuildSubmitinfo(); //Clean up maybe move to Get
		~RenderInfo() = default;
	};
}