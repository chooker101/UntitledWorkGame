#pragma once

namespace hal
{
	class RenderPass;
	class FramebufferLayout
	{
	private:
		VkFramebufferCreateInfo mFrameBufferCI = {};
		std::vector<VkImageView> vAttachments = {};
	public:
		FramebufferLayout(uint32_t width, uint32_t height);
		void AppendViewAttachment(const VkImageView& imageView);
		void InsertViewAttachment(const VkImageView& imageView, uint32_t index);
		void SetRenderPass(const RenderPass& renderPass);
		const VkFramebufferCreateInfo& GetFrameBufferCI() const;
	};
}