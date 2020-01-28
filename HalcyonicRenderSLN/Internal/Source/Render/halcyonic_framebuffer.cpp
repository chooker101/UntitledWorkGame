#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Render/halcyonic_framebuffer_layout.hpp>
#include <Render/halcyonic_framebuffer.hpp>

using namespace hal;

hal::Framebuffer::Framebuffer(const FramebufferLayout* framebufferLayout) : mFramebufferLayout(framebufferLayout)
{
	HALCYONIC_DEBUG((mFramebufferLayout->GetFrameBufferCI().attachmentCount > 0), "FramebufferLayout: There are no vkImageView attachments")
	HALCYONIC_VK_CHECK(vkCreateFramebuffer(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mFramebufferLayout->GetFrameBufferCI(), nullptr, &mFrameBuffer), "Framebuffer: Could not create Frambuffer.");
}

const VkFramebuffer & hal::Framebuffer::GetVulkanFramebuffer() const
{
	return mFrameBuffer;
}
