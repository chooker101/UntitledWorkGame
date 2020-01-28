#include<precompiled.hpp>
#include<Render/halcyonic_swapchain_placeholder_layout.hpp>

hal::SwapchainColourPlaceholder::SwapchainColourPlaceholder(const VkFormat& colourFormat)
{
	mAttachmentDescription.format = colourFormat;
	mAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	mAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	mAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	mAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	mAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	mAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	mAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}
