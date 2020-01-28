#include<precompiled.hpp>
#include<Render/halcyonic_renderpass.hpp>
#include<Render/halcyonic_framebuffer_layout.hpp>

using namespace hal;

hal::FramebufferLayout::FramebufferLayout(uint32_t width, uint32_t height)
{
	mFrameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	mFrameBufferCI.pNext = nullptr;
	mFrameBufferCI.attachmentCount = vAttachments.size();
	mFrameBufferCI.pAttachments = vAttachments.data();
	mFrameBufferCI.width = width;
	mFrameBufferCI.height = height;
	mFrameBufferCI.layers = 1;
}

void hal::FramebufferLayout::AppendViewAttachment(const VkImageView & imageView)
{
	vAttachments.push_back(imageView);
	mFrameBufferCI.attachmentCount = vAttachments.size();
	mFrameBufferCI.pAttachments = vAttachments.data();
}

void hal::FramebufferLayout::InsertViewAttachment(const VkImageView & imageView, uint32_t index)
{
	HALCYONIC_DEBUG(index > vAttachments.size(), "FramebufferLayout: View attachment index out of range")
	vAttachments[index] = imageView;
	mFrameBufferCI.pAttachments = vAttachments.data();
}

void hal::FramebufferLayout::SetRenderPass(const RenderPass & renderPass)
{
	mFrameBufferCI.renderPass = renderPass.GetVulkanRenderPass();
}

const VkFramebufferCreateInfo & hal::FramebufferLayout::GetFrameBufferCI() const
{
	return mFrameBufferCI;
}
