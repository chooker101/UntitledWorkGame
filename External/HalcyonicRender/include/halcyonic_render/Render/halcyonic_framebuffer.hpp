#pragma once

namespace hal
{
	class FramebufferLayout;
	class Framebuffer
	{
	private:
		const FramebufferLayout* mFramebufferLayout;
		VkFramebuffer mFrameBuffer;
	public:
		Framebuffer(const FramebufferLayout* framebufferLayout);
		const VkFramebuffer& GetVulkanFramebuffer() const;
	};
}