#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Pipeline/halcyonic_descriptor_pool.hpp>
#include <Pipeline/halcyonic_pipeline_layout.hpp>
#include <Pipeline/halcyonic_pipeline.hpp>
#ifndef __ANDROID__
#include <fstream>
#endif

using namespace hal;

void Pipeline::LoadShader(VkPipelineShaderStageCreateInfo& shaderStage, std::string filename, VkShaderStageFlagBits stage) //Fix/Remove this is gross
{
	shaderStage = {};

	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage = stage;

	//Create Module
	size_t size;
	char* shaderCode;

#ifdef _WIN32
	std::fstream stream;
	stream.open(filename.c_str(), std::fstream::in | std::fstream::binary);
	assert(stream.is_open());

	stream.seekg(0, std::ios::end);
	size = static_cast<size_t>(stream.tellg());
	assert(size > 0);
	stream.seekg(0, std::ios::beg);

	shaderCode = new char[size];
	stream.read(shaderCode, size);
	assert(stream);
	stream.close();
#elif defined (__ANDROID__)
	AAsset* asset = AAssetManager_open(Application::Instance()->GetApp()->activity->assetManager, filename.c_str(), AASSET_MODE_BUFFER);
	assert(asset);
	size = AAsset_getLength(asset);
	assert(size > 0);

	shaderCode = new char[size];
	AAsset_read(asset, shaderCode, size);
	AAsset_close(asset);
#endif

	VkShaderModuleCreateInfo moduleCreateInfo = {};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.codeSize = size;
	moduleCreateInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode);
	moduleCreateInfo.flags = 0;

	HALCYONIC_VK_CHECK(vkCreateShaderModule(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &moduleCreateInfo, nullptr, &shaderStage.module),"Pipeline: Failed to create shader module.");
	delete[] shaderCode;
	HALCYONIC_DEBUG(shaderStage.module != VK_NULL_HANDLE, "Pipeline: Shader module is null");

	shaderStage.pName = "main"; //add ability to change
}

void Pipeline::BuildPipelines()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	HALCYONIC_VK_CHECK(vkCreatePipelineCache(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &mVulkanPipelineCache),"Pipeline: Could not create Pipeline Cache");

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = mPipelineLayout->mDescriptorPool->GetVKDescriptorSetLayout();
	HALCYONIC_VK_CHECK(vkCreatePipelineLayout(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &mVulkanPipelineLayout),"Pipeline: Could not create Pipeline Layout");

	mPipelineLayout->mGraphicsPipelineCI.layout = mVulkanPipelineLayout;
	mPipelineLayout->mGraphicsPipelineCI.basePipelineHandle = mVulkanPipeline;
	mPipelineLayout->mGraphicsPipelineCI.basePipelineIndex = -1;
	HALCYONIC_VK_CHECK(vkCreateGraphicsPipelines(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mVulkanPipelineCache, 1, &mPipelineLayout->mGraphicsPipelineCI, nullptr, &mVulkanPipeline),"Pipeline: Could not create Graphics Pipeline. This can be a number of things.");
}

Pipeline::Pipeline(PipelineLayout* pipelineLayout) : mPipelineLayout(pipelineLayout)
{
	for (uint32_t i = 0; i < static_cast<uint32_t>(pipelineLayout->mShaderPaths.size()); ++i)
	{
		LoadShader(pipelineLayout->mShaderStageCIs[i], pipelineLayout->mShaderPaths[i]->mPath, static_cast<VkShaderStageFlagBits>(pipelineLayout->mShaderPaths[i]->mStage));
	}
	
	BuildPipelines();
}