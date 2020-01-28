#pragma once

namespace hal
{
	class ShaderInputLayout;
	class DescriptorPool;
	class RenderPass;
	enum class ShaderStage;

	struct ShaderInfo
	{
		ShaderStage mStage;
		std::string mPath;
	};

	class PipelineLayout
	{
	private:
		friend class Pipeline;

		const ShaderInputLayout* mShaderInputLayout;
		std::vector<const ShaderInfo*> mShaderPaths;
		const DescriptorPool* mDescriptorPool = nullptr;
		const RenderPass* mRenderPass;

		VkPipelineInputAssemblyStateCreateInfo mInputAssemblyStateCI = {};
		VkPipelineRasterizationStateCreateInfo mRasterizationStateCI = {};
		std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentStateCIs = {};
		VkPipelineColorBlendStateCreateInfo mColorBlendStateCI = {};
		VkPipelineViewportStateCreateInfo mViewPortStateCI = {};
		std::vector<VkDynamicState> mDynamicStateCIs = {};
		VkPipelineDynamicStateCreateInfo mPipelineDynamicStateCI = {};
		VkPipelineDepthStencilStateCreateInfo mDepthStencilStateCI = {};
		VkPipelineMultisampleStateCreateInfo mMultisapleStateCI = {};
		std::vector<VkPipelineShaderStageCreateInfo> mShaderStageCIs = {};
		VkPipelineTessellationStateCreateInfo mTesselationStateCI = {};
		VkGraphicsPipelineCreateInfo mGraphicsPipelineCI = {};
		VkPipelineVertexInputStateCreateInfo mInputStateCI = {};
		VkDescriptorSetLayoutCreateInfo mDescriptorLayoutCI = {};

		uint32_t mInputBindingCount = 0; //For expansion to multiple bindings
		VkVertexInputBindingDescription mInputBinding = {};
		std::vector<VkVertexInputAttributeDescription> mInputAttributes = {};
		std::vector<VkDescriptorSetLayoutBinding> mDescriptorSetBindings = {};

		void BuildInputState();
		void BuildDescriptorLayout();
		void PrepareDefaultCreateInfos();
	public:
		//!Create a pipeline layout(Order of lists is usage order)
		PipelineLayout(std::vector<const ShaderInfo*> shaderPaths, const ShaderInputLayout* shaderInput);

		//Add Sets for all create infos
		void SetDescriptorPool(const DescriptorPool* descriptorPool);
		void SetRenderPass(const RenderPass* renderPass);

		const VkDescriptorSetLayoutCreateInfo* GetDescriptorSetLayoutCI() { return &mDescriptorLayoutCI; }

		~PipelineLayout();
	};
}