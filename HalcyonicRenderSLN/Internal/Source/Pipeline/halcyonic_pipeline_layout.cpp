#include <precompiled.hpp>
#include <Render/halcyonic_renderpass.hpp>
#include <Pipeline/halcyonic_descriptor_pool.hpp>
#include <Pipeline/halcyonic_shader_input_layout.hpp>
#include <Pipeline/halcyonic_pipeline_layout.hpp>

namespace hal
{
	void PipelineLayout::BuildInputState()
	{
		mInputBinding.binding = mInputBindingCount;
		mInputBinding.stride = mShaderInputLayout->GetInputStride();
		mInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		mInputAttributes.resize(mShaderInputLayout->GetInputAttributesSize());
		for (uint32_t i = 0; i < mShaderInputLayout->GetInputAttributesSize(); ++i)
		{
			mInputAttributes[i].binding = mInputBindingCount;
			mInputAttributes[i].location = i;
			mInputAttributes[i].format = mShaderInputLayout->GetInputAttribute(i)->mFormat;
			mInputAttributes[i].offset = mShaderInputLayout->GetInputAttribute(i)->mOffset;
		}

		mInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		mInputStateCI.pNext = nullptr;
		mInputStateCI.flags = 0;
		mInputStateCI.vertexBindingDescriptionCount = 1;
		mInputStateCI.pVertexBindingDescriptions = &mInputBinding;
		mInputStateCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(mInputAttributes.size());
		mInputStateCI.pVertexAttributeDescriptions = mInputAttributes.data();
	}

	void PipelineLayout::BuildDescriptorLayout()
	{
		mDescriptorSetBindings.resize(mShaderInputLayout->GetDescriptorSetLayoutsSize());

		for (uint32_t i = 0; i < mDescriptorSetBindings.size(); ++i)
		{
			mDescriptorSetBindings[i].binding = i;
			mDescriptorSetBindings[i].descriptorType = static_cast<VkDescriptorType>(mShaderInputLayout->GetDescriptorSetLayout(i)->GetLayoutBindingDescriptor());
			mDescriptorSetBindings[i].descriptorCount = 1;
			mDescriptorSetBindings[i].stageFlags = static_cast<VkShaderStageFlags>(mShaderInputLayout->GetDescriptorSetLayout(i)->GetShaderStage());
			mDescriptorSetBindings[i].pImmutableSamplers = nullptr;
		}

		mDescriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		mDescriptorLayoutCI.pNext = nullptr;
		mDescriptorLayoutCI.bindingCount = static_cast<uint32_t>(mDescriptorSetBindings.size());
		mDescriptorLayoutCI.pBindings = mDescriptorSetBindings.data();
	}

	void PipelineLayout::PrepareDefaultCreateInfos()
	{
		mInputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		mInputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		mRasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		mRasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
		mRasterizationStateCI.cullMode = VK_CULL_MODE_BACK_BIT;
		mRasterizationStateCI.frontFace = VK_FRONT_FACE_CLOCKWISE;
		mRasterizationStateCI.depthClampEnable = VK_FALSE;
		mRasterizationStateCI.rasterizerDiscardEnable = VK_FALSE;
		mRasterizationStateCI.depthBiasEnable = VK_FALSE;
		mRasterizationStateCI.lineWidth = 1.0f;

		mBlendAttachmentStateCIs.resize(1);
		mBlendAttachmentStateCIs[0].colorWriteMask = 0xf;
		mBlendAttachmentStateCIs[0].blendEnable = VK_FALSE;

		mColorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		mColorBlendStateCI.attachmentCount = 1;
		mColorBlendStateCI.pAttachments = mBlendAttachmentStateCIs.data();

		mViewPortStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		mViewPortStateCI.viewportCount = 1;
		mViewPortStateCI.scissorCount = 1;

		mDynamicStateCIs.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		mDynamicStateCIs.push_back(VK_DYNAMIC_STATE_SCISSOR);

		mPipelineDynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		mPipelineDynamicStateCI.pDynamicStates = mDynamicStateCIs.data();
		mPipelineDynamicStateCI.dynamicStateCount = static_cast<uint32_t>(mDynamicStateCIs.size());

		mDepthStencilStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		mDepthStencilStateCI.depthTestEnable = VK_TRUE;
		mDepthStencilStateCI.depthWriteEnable = VK_TRUE;
		mDepthStencilStateCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		mDepthStencilStateCI.depthBoundsTestEnable = VK_FALSE;
		mDepthStencilStateCI.back.failOp = VK_STENCIL_OP_KEEP;
		mDepthStencilStateCI.back.passOp = VK_STENCIL_OP_KEEP;
		mDepthStencilStateCI.back.compareOp = VK_COMPARE_OP_ALWAYS;
		mDepthStencilStateCI.stencilTestEnable = VK_FALSE;
		mDepthStencilStateCI.front = mDepthStencilStateCI.back;

		mMultisapleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		mMultisapleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		mMultisapleStateCI.pSampleMask = nullptr;

		mShaderStageCIs.resize(mShaderPaths.size());

		mGraphicsPipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		mGraphicsPipelineCI.pNext = nullptr;
		mGraphicsPipelineCI.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
		mGraphicsPipelineCI.stageCount = static_cast<uint32_t>(mShaderStageCIs.size());
		mGraphicsPipelineCI.pStages = mShaderStageCIs.data();
		mGraphicsPipelineCI.pInputAssemblyState = &mInputAssemblyStateCI;
		mGraphicsPipelineCI.pRasterizationState = &mRasterizationStateCI;
		mGraphicsPipelineCI.pColorBlendState = &mColorBlendStateCI;
		mGraphicsPipelineCI.pTessellationState = nullptr;
		mGraphicsPipelineCI.pMultisampleState = &mMultisapleStateCI;
		mGraphicsPipelineCI.pViewportState = &mViewPortStateCI;
		mGraphicsPipelineCI.pDepthStencilState = &mDepthStencilStateCI;
		mGraphicsPipelineCI.pDynamicState = &mPipelineDynamicStateCI;
		mGraphicsPipelineCI.pVertexInputState = &mInputStateCI;
	}

	PipelineLayout::PipelineLayout(std::vector<const ShaderInfo*> shaderPaths, const ShaderInputLayout * shaderInput) :
		mShaderPaths(std::move(shaderPaths)),
		mShaderInputLayout(shaderInput)
	{
		HALCYONIC_DEBUG(mShaderInputLayout, "PipelineLayout: Shader Input Layout not set");
		HALCYONIC_DEBUG(mShaderPaths.size() != 0, "PipelineLayout: Shader Paths not set");

		BuildInputState();
		BuildDescriptorLayout();
		PrepareDefaultCreateInfos();
	}

	void PipelineLayout::SetDescriptorPool(const DescriptorPool * descriptorPool)
	{
		mDescriptorPool = descriptorPool;
	}

	void PipelineLayout::SetRenderPass(const RenderPass * renderPass)
	{
		mRenderPass = renderPass;
		mGraphicsPipelineCI.renderPass = mRenderPass->GetVulkanRenderPass();
	}

	PipelineLayout::~PipelineLayout()
	{
		for (auto& shaderPath : mShaderPaths)
		{
			delete shaderPath;
		}
		mShaderPaths.clear();
		delete mShaderInputLayout;
	}
}