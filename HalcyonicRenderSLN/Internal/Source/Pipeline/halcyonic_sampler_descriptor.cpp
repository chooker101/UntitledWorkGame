#include <precompiled.hpp>
#include <Pipeline/halcyonic_image_sampler.hpp>
#include <Pipeline/halcyonic_sampler_descriptor.hpp>

using namespace hal;

hal::SamplerDescriptor::SamplerDescriptor(const DescriptorLayout* descriptorLayout, const ImageSampler* imageSampler): Descriptor(descriptorLayout), mImageSampler(imageSampler)
{
	mDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	mDescriptorImageInfo.imageView = mImageSampler->GetVKImageView();
	mDescriptorImageInfo.sampler = mImageSampler->GetVKSampler();
}
