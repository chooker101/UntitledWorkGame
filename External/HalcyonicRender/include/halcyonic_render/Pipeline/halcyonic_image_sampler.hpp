#pragma once
#include <Pipeline/halcyonic_image_sampler_layout.hpp>

namespace hal
{
	class Buffer;
	class SetupCommandBuffer;
	class ImageSampler
	{
	private:
		ImageSamplerLayout* mImageSamplerLayout;
		VkSampler mSampler;
		VkImage mImage; //Maybe split to texture class
		VkImageLayout mImageLayout;
		VkImageView mImageView;
		VkDeviceMemory mDeviceMemory; //Should be in vma
		void SetImageLayout(VkCommandBuffer cmdBuffer, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange);
	public:
		ImageSampler(const SetupCommandBuffer& setupCommandBuffer, uint32_t size, uint8_t* data, ImageSamplerLayout* imageSamplerLayout);
		const VkImageView& GetVKImageView() const { return mImageView; }
		const VkSampler& GetVKSampler() const { return mSampler; }
	};
}