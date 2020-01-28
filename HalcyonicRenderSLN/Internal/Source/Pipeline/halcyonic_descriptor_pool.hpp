#pragma once
#include <Pipeline/halcyonic_descriptor.hpp>

namespace hal
{
	class PipelineLayout;
	class DescriptorPool
	{
	private:
		std::unordered_map<LayoutBindingDescriptor,std::vector<const Descriptor*>> mDescriptorSets;
		VkDescriptorPoolCreateInfo mDescriptorPoolCI;
		VkDescriptorPoolSize mDescriptorPoolSize;
		VkDescriptorPool mDescriptorPool;
		VkDescriptorSetAllocateInfo mDescriptorAllocInfo;
		VkDescriptorSet mDescriptorSet;
		VkDescriptorSetLayout mVulkanDescriptorLayout;
		uint32_t mSetSize;
	public:
		DescriptorPool(std::vector<const Descriptor*> descriptorSets, PipelineLayout* pipelineLayout);
		const VkDescriptorSet& GetVKDescriptorSet() const { return mDescriptorSet; }
		const VkDescriptorSetLayout* GetVKDescriptorSetLayout() const { return &mVulkanDescriptorLayout; }
	};
}