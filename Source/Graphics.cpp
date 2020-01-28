#include "precompiled.hpp"
#include "Application.hpp"
#include "Vector3.hpp"
#include "Matrix4.hpp"
#include "Camera.hpp"
#include "RenderVertex.hpp"
#include "RenderObject.hpp"
#include "Graphics.hpp"

graphics_ptr Graphics::s_Instance = nullptr;

Graphics::Graphics()
{
	hal::Render::CreateInstance();
	hal::Render::Instance()->SetRenderLayout(&mRenderLayout);
	hal::Render::Instance()->InitializeVulkan(Application::Instance()->GetInstanceHandle(), Application::Instance()->GetWindowHandle());

	mCommandPool = new hal::CommandPool(hal::Render::Instance()->GetSwapchain().GetQueueFamilyIndex());
	mSetupCommandBuffer = new hal::SetupCommandBuffer(mCommandPool);
	mSetupCommandBuffer->StartSetupBuffer();

	mSwapchainPlaceholder = new hal::SwapchainColourPlaceholder(hal::Render::Instance()->GetColourFormat());
	mSwapchainDepthStencilLayout = new hal::DepthStencilLayout();
	mSwapchainAttachments = { mSwapchainPlaceholder, mSwapchainDepthStencilLayout };
	mSwapchainDepthStencil = new hal::DepthStencil(mSwapchainDepthStencilLayout);

	mRenderPassLayout = new hal::RenderPassLayout(mSwapchainAttachments);
	mRenderPass = new hal::RenderPass(mRenderPassLayout);

	mDescriptorLayouts = { new hal::DescriptorLayout(hal::ShaderStage::Vertex, hal::LayoutBindingDescriptor::UniformBuffer, 0) };

	mInputVector = { new hal::InputAttributes(VK_FORMAT_R32G32B32_SFLOAT, offsetof(RenderVertex, RenderVertex::mPosition)), new hal::InputAttributes(VK_FORMAT_R32G32B32_SFLOAT, offsetof(RenderVertex, RenderVertex::mColor)) }; //Store array
	mShaderInputLayout = new hal::ShaderInputLayout(mInputVector, sizeof(RenderVertex), mDescriptorLayouts);
	mShaderInfos = { new hal::ShaderInfo{hal::ShaderStage::Vertex, std::string("vertex.vert.spv")}, new hal::ShaderInfo{hal::ShaderStage::Fragment,  std::string("frag.frag.spv")} };
	mPipelineLayout = new hal::PipelineLayout(mShaderInfos, mShaderInputLayout);
	mPipelineLayout->SetRenderPass(mRenderPass); //Move to constructor

	mMatricesBuffer = new hal::Buffer(sizeof(TransformMatracies), reinterpret_cast<uint8_t*>(&mTransformMatracies), hal::BufferType::UniformBuffer);
	mMatraciesDescriptor = new hal::BufferDescriptor(mDescriptorLayouts[0], mMatricesBuffer);
	mPipelineDescriptors = { mMatraciesDescriptor };
	mDescriptorPool = new hal::DescriptorPool(mPipelineDescriptors, mPipelineLayout);

	mPipeline = new hal::Pipeline(mPipelineLayout);

	hal::Render::Instance()->InitializeRender(mRenderPass, mSwapchainDepthStencil);
	mSetupCommandBuffer->EndAndSubmitSetupBuffer();

	hal::Render::Instance()->AddRenderInfo(&mRenderInfo);
}

void Graphics::CreateInstance()
{
	if (s_Instance == nullptr)
	{
		s_Instance = static_cast<graphics_ptr>(new Graphics());
	}
}

const graphics_ptr & Graphics::Instance()
{
	assert(s_Instance != nullptr);
	return s_Instance;
}

void Graphics::DestroyInstance()
{
	s_Instance.release();
	s_Instance = nullptr;
}

void Graphics::AddRenderObject(RenderObject* renderObject)
{
	mRenderObjects.push_back(renderObject);
	mRenderInfo.AddDrawBuffer(renderObject->GetDrawBuffer()); 
}

void Graphics::Draw()
{
	mTransformMatracies.mViewMatrix = mMainCamera->GetView();
	mTransformMatracies.mProjectionMatrix = mMainCamera->GetProjection();
	for (auto renderObject : mRenderObjects)
	{
		hal::DrawBuffer* drawBuffer = renderObject->GetDrawBuffer();
		mTransformMatracies.mModelMatrix = renderObject->GetModelMatrix();
		mMatricesBuffer->UpdateBuffer(reinterpret_cast<uint8_t*>(&mTransformMatracies));

		drawBuffer->StartDrawBuffer();
		hal::Render::Instance()->BeginRenderPass(*drawBuffer);
		drawBuffer->RecordVulkanCommands(vkCmdBindDescriptorSets, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->GetVKPipelineLayout(), 0, 1, &mDescriptorPool->GetVKDescriptorSet(), 0, nullptr); //Pipeline placeholder?

		drawBuffer->RecordVulkanCommands(vkCmdBindPipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->GetVKPipeline());

		drawBuffer->RecordVulkanCommands(vkCmdBindVertexBuffers, 0, 1, hal::DrawCommand::GetBufferPlaceholder(hal::BufferType::VertexBuffer, 0), mBufferOffsets);

		drawBuffer->RecordVulkanCommands(vkCmdBindIndexBuffer, hal::DrawCommand::GetBufferPlaceholder(hal::BufferType::IndexBuffer, 0), 0, VK_INDEX_TYPE_UINT32);

		drawBuffer->RecordVulkanCommands(vkCmdDrawIndexed, hal::DrawCommand::GetBufferLengthPlaceholder(hal::BufferType::IndexBuffer, 0), 1, 0, 0, 0);
		hal::Render::Instance()->EndRenderPass(*drawBuffer);
		drawBuffer->EndDrawBuffer();
	}

	//mRenderInfo.BuildRenderinfo();
	mRenderInfo.BuildSubmitinfo();
	hal::Render::Instance()->Submit();
}

Graphics::~Graphics()
{
	//ToDo add deletes also smart pointer instead
}
