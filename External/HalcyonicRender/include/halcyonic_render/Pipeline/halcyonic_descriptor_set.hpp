#pragma once
#include <Pipeline/halcyonic_descriptor_set_layout.hpp>

namespace hal
{
	class DescriptorSetLayout;
	class DescriptorSet //Rename to descriptor
	{
	protected:
		const DescriptorSetLayout* mDescriptorSetLayout;
		VkDescriptorBufferInfo mDescriptorBufferInfo = {};
		DescriptorSet(const DescriptorSetLayout* descriptorSetLayout) : mDescriptorSetLayout(descriptorSetLayout) {}
	public:
		LayoutBindingDescriptor GetLayoutBindingDescriptor() const { return mDescriptorSetLayout->GetLayoutBindingDescriptor(); }
		const DescriptorSetLayout* GetDescriptorLayout() const { return mDescriptorSetLayout; }
		const VkDescriptorBufferInfo& GetDescriptorBufferInfo() const { return mDescriptorBufferInfo; }
	};
}