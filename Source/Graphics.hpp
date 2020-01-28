#pragma once
#include <halcyonic_renderer.hpp>
#include "Matrix4.hpp"

class Camera;
class RenderObject;
class Graphics;
typedef std::unique_ptr<Graphics> graphics_ptr;
class Graphics
{
private:
	static graphics_ptr s_Instance;
	struct TransformMatracies
	{
		Matrix4 mModelMatrix;
		Matrix4 mViewMatrix;
		Matrix4 mProjectionMatrix;
	} mTransformMatracies; //These should really be in seperate ubos

	hal::RenderLayout mRenderLayout;
	hal::CommandPool* mCommandPool;
	hal::SetupCommandBuffer* mSetupCommandBuffer;
	hal::SwapchainColourPlaceholder* mSwapchainPlaceholder;
	hal::DepthStencilLayout* mSwapchainDepthStencilLayout;
	std::vector<hal::AttachmentLayout*> mSwapchainAttachments;
	hal::DepthStencil* mSwapchainDepthStencil;
	hal::RenderPassLayout* mRenderPassLayout;
	hal::RenderPass* mRenderPass;
	std::vector<const hal::DescriptorLayout*> mDescriptorLayouts;
	std::vector<const hal::InputAttributes*> mInputVector;
	hal::ShaderInputLayout* mShaderInputLayout;
	std::vector<const hal::ShaderInfo*> mShaderInfos;
	hal::PipelineLayout* mPipelineLayout;
	hal::Buffer* mMatricesBuffer;
	hal::BufferDescriptor* mMatraciesDescriptor;
	std::vector<const hal::Descriptor*> mPipelineDescriptors;
	hal::DescriptorPool* mDescriptorPool;
	hal::Pipeline* mPipeline;
	hal::RenderInfo mRenderInfo;

	VkDeviceSize mBufferOffsets[1] = { 0 };

	std::vector<RenderObject*> mRenderObjects;

	Camera* mMainCamera;

	Graphics();
public:
	static void CreateInstance();
	static const graphics_ptr& Instance();
	static void DestroyInstance();

	const hal::Pipeline* GetPipeline() const { return mPipeline; }
	const hal::CommandPool* GetCommandPool() const { return mCommandPool; }
	Camera& GetMainCamera() const { return *mMainCamera; }

	void AddRenderObject(RenderObject* renderObject); //OOF
	void SetMainCamera(Camera* mainCamera) { mMainCamera = mainCamera; }

	void RebuildRenderInfo() { mRenderInfo.BuildRenderinfo(); }
	void Draw();

	~Graphics();
};