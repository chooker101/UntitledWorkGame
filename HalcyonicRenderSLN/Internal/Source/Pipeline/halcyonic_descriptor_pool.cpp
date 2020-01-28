#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Pipeline/halcyonic_pipeline.hpp>
#include <Pipeline/halcyonic_pipeline_layout.hpp>
#include <Pipeline/halcyonic_buffer_descriptor.hpp>
#include <Pipeline/halcyonic_sampler_descriptor.hpp>
#include <Pipeline/halcyonic_descriptor_pool.hpp>

using namespace hal;

hal::DescriptorPool::DescriptorPool(std::vector<const Descriptor*> descriptorSets, PipelineLayout* pipelineLayout) : mSetSize(descriptorSets.size())
{
	HALCYONIC_VK_CHECK(vkCreateDescriptorSetLayout(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), pipelineLayout->GetDescriptorSetLayoutCI(), nullptr, &mVulkanDescriptorLayout), "Pipeline: Could not create Descriptor Set Layout");
	pipelineLayout->SetDescriptorPool(this);

	for (auto ds : descriptorSets)
	{
		mDescriptorSets[ds->GetLayoutBindingDescriptor()].push_back(ds);
	}

	std::vector<VkDescriptorPoolSize> typeCounts;
	typeCounts.resize(mDescriptorSets.size());
	auto typeCountIterator = typeCounts.begin();

	for (auto setTypePair : mDescriptorSets)
	{
		typeCountIterator->type = static_cast<VkDescriptorType>(setTypePair.first);
		typeCountIterator->descriptorCount = setTypePair.second.size();
		typeCountIterator++;
	}

	mDescriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	mDescriptorPoolCI.pNext = nullptr;
	mDescriptorPoolCI.poolSizeCount = static_cast<uint32_t>(typeCounts.size());
	mDescriptorPoolCI.pPoolSizes = typeCounts.data();
	mDescriptorPoolCI.maxSets = mSetSize;

	HALCYONIC_VK_CHECK(vkCreateDescriptorPool(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mDescriptorPoolCI, nullptr, &mDescriptorPool), "DescriptorPool: Could not allocate descriptor pool");

	mDescriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	mDescriptorAllocInfo.descriptorPool = mDescriptorPool;
	mDescriptorAllocInfo.descriptorSetCount = 1;
	mDescriptorAllocInfo.pSetLayouts = &mVulkanDescriptorLayout;

	HALCYONIC_VK_CHECK(vkAllocateDescriptorSets(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mDescriptorAllocInfo, &mDescriptorSet), "DescriptorPool: Could not allocate descriptor set");

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.resize(mSetSize);
	auto writeDescriptorIterator = writeDescriptorSets.begin();

	for (auto setTypePair : mDescriptorSets)
	{
		for (auto ds : setTypePair.second)
		{
			writeDescriptorIterator->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorIterator->dstSet = mDescriptorSet;
			writeDescriptorIterator->descriptorCount = 1;
			writeDescriptorIterator->descriptorType = static_cast<VkDescriptorType>(setTypePair.first);
			if (setTypePair.first == LayoutBindingDescriptor::UniformBuffer)
			{
				writeDescriptorIterator->pBufferInfo = &reinterpret_cast<const BufferDescriptor*>(ds)->GetDescriptorBufferInfo();
			}
			else if(setTypePair.first == LayoutBindingDescriptor::ImageSampler)
			{
				writeDescriptorIterator->pImageInfo = &reinterpret_cast<const SamplerDescriptor*>(ds)->GetDescriptorImageInfo();
			}
			writeDescriptorIterator->dstBinding = ds->GetDescriptorLayout()->GetBindingLocation();
			writeDescriptorIterator++;
		}
	}

	vkUpdateDescriptorSets(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr); //need a way to be able to update sets
}