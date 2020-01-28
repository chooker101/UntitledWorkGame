#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Pipeline/halcyonic_image_sampler_layout.hpp>

hal::ImageSamplerLayout::ImageSamplerLayout(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat colorFormat) : mWidth(width), mHeight(height), mMipLevels(mipLevels), mColorFormat(colorFormat)
{
	mImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	mImageCI.pNext = NULL;
	mImageCI.imageType = VK_IMAGE_TYPE_2D;
	mImageCI.format = colorFormat;
	mImageCI.mipLevels = mipLevels;
	mImageCI.arrayLayers = 1;
	mImageCI.samples = VK_SAMPLE_COUNT_1_BIT;
	mImageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
	mImageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	mImageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mImageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mImageCI.extent = { width, height, 1 };
	mImageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

	mImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	mImageViewCI.pNext = NULL;
	mImageViewCI.image = VK_NULL_HANDLE;
	mImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	mImageViewCI.format = colorFormat;
	mImageViewCI.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	mImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	mImageViewCI.subresourceRange.baseMipLevel = 0;
	mImageViewCI.subresourceRange.baseArrayLayer = 0;
	mImageViewCI.subresourceRange.layerCount = 1;
	mImageViewCI.subresourceRange.levelCount = mipLevels;

	mSamplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	mSamplerCI.pNext = NULL;
	mSamplerCI.magFilter = VK_FILTER_LINEAR;
	mSamplerCI.minFilter = VK_FILTER_LINEAR;
	mSamplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	mSamplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	mSamplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	mSamplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	mSamplerCI.mipLodBias = 0.0f;
	mSamplerCI.compareOp = VK_COMPARE_OP_NEVER;
	mSamplerCI.minLod = 0.0f;
	mSamplerCI.maxLod = static_cast<float>(mipLevels);
	if (Render::Instance()->GetVulkanDevice().GetPhysicalDeviceFeatures().samplerAnisotropy)
	{
		mSamplerCI.maxAnisotropy = Render::Instance()->GetVulkanDevice().GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		mSamplerCI.anisotropyEnable = VK_TRUE;
	}
	else
	{
		mSamplerCI.maxAnisotropy = 1.0;
		mSamplerCI.anisotropyEnable = VK_FALSE;
	}
	mSamplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
}

void hal::ImageSamplerLayout::SetImage(const VkImage& image)
{
	mImageViewCI.image = image;
}
