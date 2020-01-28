#pragma once

namespace hal
{
	class ImageSamplerLayout
	{
	private:
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mMipLevels;
		VkFormat mColorFormat;
		VkImageCreateInfo mImageCI = {}; //Make combined image class
		VkImageViewCreateInfo mImageViewCI = {};
		VkSamplerCreateInfo mSamplerCI = {};
	public:
		ImageSamplerLayout(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat colorFormat);

		const VkImageCreateInfo& GetImageCI() const { return mImageCI; }
		const VkImageViewCreateInfo& GetImageViewCI() const { return mImageViewCI; }
		const VkSamplerCreateInfo& GetSamplerCI() const { return mSamplerCI; }
		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		uint32_t GetMipLevels() const { return mMipLevels; }

		void SetImage(const VkImage& image);
	};
}