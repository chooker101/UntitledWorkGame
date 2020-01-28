#include<precompiled.hpp>
#include<Render/halcyonic_render.hpp>
#include<Render/halcyonic_colour_layout.hpp>

using namespace hal;

hal::ColourLayout::ColourLayout(const VkFormat& colourFormat)
{
	mImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	mImageCI.pNext = nullptr;
	mImageCI.imageType = VK_IMAGE_TYPE_2D;
	mImageCI.format = colourFormat;
	mImageCI.extent.width = Render::Instance()->GetCurrentWidth();
	mImageCI.extent.height = Render::Instance()->GetCurrentHeight();
	mImageCI.extent.depth = 1;
	mImageCI.mipLevels = 1;
	mImageCI.arrayLayers = 1;
	mImageCI.samples = VK_SAMPLE_COUNT_1_BIT;
	mImageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
	mImageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	mImageCI.flags = 0;
	mImageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	mImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	mImageViewCI.pNext = nullptr;
	mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	mImageViewCI.format = colourFormat;
	mImageViewCI.flags = 0;
	mImageViewCI.subresourceRange = {};
	mImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	mImageViewCI.subresourceRange.baseMipLevel = 0;
	mImageViewCI.subresourceRange.levelCount = 1;
	mImageViewCI.subresourceRange.baseArrayLayer = 0;
	mImageViewCI.subresourceRange.layerCount = 1;

	mAttachmentDescription.format = colourFormat;
	mAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	mAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	mAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	mAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	mAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	mAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	mAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

const VkImageCreateInfo & hal::ColourLayout::GetImageCreateInfo() const
{
	return mImageCI;
}

const VkImageViewCreateInfo & hal::ColourLayout::GetViewCreateInfo() const
{
	return mImageViewCI;
}

void hal::ColourLayout::SetViewCreateInfoImage(const VkImage & image)
{
	mImageViewCI.image = image;
}
