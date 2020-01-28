#pragma once

namespace hal
{
	class PipelineLayout;
	
	class Pipeline
	{
	private:
		PipelineLayout* mPipelineLayout = nullptr;

		VkPipelineCache mVulkanPipelineCache;
		VkPipelineLayout mVulkanPipelineLayout;
		VkPipeline mVulkanPipeline;
		//VkDescriptorSetLayout mVulkanDescriptorLayout;

		static void LoadShader(VkPipelineShaderStageCreateInfo& shaderStage, std::string filename, VkShaderStageFlagBits stage);
		void BuildPipelines();
	public:
		Pipeline(PipelineLayout* pipelineLayout);

		const PipelineLayout& GetPipelineLayout() const { return *mPipelineLayout; }

		//const VkDescriptorSetLayout& GetDescriptorLayout() const { return mVulkanDescriptorLayout; }
		const VkPipelineLayout& GetVKPipelineLayout() const { return mVulkanPipelineLayout; }
		const VkPipeline& GetVKPipeline() const { return mVulkanPipeline; }
	};
}