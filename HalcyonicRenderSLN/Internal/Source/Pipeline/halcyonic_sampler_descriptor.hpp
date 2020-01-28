#pragma once
#include <Pipeline/halcyonic_descriptor.hpp>

namespace hal
{
	class ImageSampler;
	class SamplerDescriptor : public Descriptor
	{
	private:
		const ImageSampler* mImageSampler;
		VkDescriptorImageInfo mDescriptorImageInfo = {};
	public:
		SamplerDescriptor(const DescriptorLayout* descriptorLayout, const ImageSampler* imageSampler);
		const VkDescriptorImageInfo& GetDescriptorImageInfo() const { return mDescriptorImageInfo; }
	};
}