#pragma once
#include <InternalVulkan/vulkan_device.hpp>
#include <Render/halcyonic_render_layout.hpp>

namespace hal
{
	class SetupCommandBuffer;
	class DrawBuffer;
	class RenderInfo;
	class RenderLayout;
	class FrameBuffer;
	class VulkanSwapChain;

	class Render;
	typedef std::unique_ptr<Render> render_ptr;

	class Render
	{
	private:
		static render_ptr s_Instance;

		std::vector<const RenderInfo*> vRenderInfos;

		const RenderLayout* mRenderLayout;
		const DepthStencil* mSwapchainDepthStencil;
		const RenderPass* mRenderPass;

		VulkanSwapChain* mSwapChain;
		VulkanDevice* mVulkanDevice;

		VkPipelineStageFlags mSubmitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkInstance mVulkanInstance;
		VkSurfaceKHR mSurface;
		VkQueue mVulkanQueue;
		VkRenderPassBeginInfo mRenderPassBeginInfo = {};
		VkRect2D mDynamicScissorState = {};
		VkViewport mVulkanViewport = {};

		VmaAllocator mAllocator;

		VkSemaphore mRenderCompleted;
		VkSemaphore mPresentCompleted;
		
		bool isRunning = false;

		uint32_t mCurrentFrame = 0;

		std::vector<VkFramebuffer> vFrameBuffers;
		std::vector<VkFence> vFences;

		Render();
		void SetupDefaultSemaphores();
		void SetupFrameBuffer();
		void PrepareSychronizationFences();
		VkResult CreateVulkanInstance();
	public:
		static void CreateInstance();
		static const render_ptr& Instance();
		static void DestroyInstance();

		//Const Ref Gets
		const VulkanDevice& GetVulkanDevice() const { return *mVulkanDevice; }
		const VkInstance& GetVulkanInstance() const { return mVulkanInstance; }
		const VkQueue& GetVulkanQueue() const { return mVulkanQueue; }
		const RenderLayout& GetRenderLayout() const { return *mRenderLayout; }
		const VkFormat& GetDepthFormat() const { return mRenderLayout->mVulkanDepthFormat; }
		const VkFormat& GetColourFormat() const { return mRenderLayout->mVulkanColourFormat; }
		const VkSemaphore& GetRenderComplete() const { return mRenderCompleted; }
		const VkSemaphore& GetPresentComplete() const { return mPresentCompleted; }

		//Reference Gets
		VkSurfaceKHR& GetVulkanSurface() { return mSurface; }
		VmaAllocator& GetAllocator() { return mAllocator; }
		VulkanSwapChain& GetSwapchain() { return *mSwapChain; }

		//Raw Gets
		uint32_t GetCurrentFrame() { return mCurrentFrame; }
		uint32_t GetCurrentHeight() { return mRenderLayout->mRenderHeight; }
		uint32_t GetCurrentWidth() { return mRenderLayout->mRenderWidth; }
		VkImage* GetCurrentImage();

		//Sets
		void SetRenderInfoSize(uint32_t size) { vRenderInfos.resize(size); }
		void AddRenderInfo(const RenderInfo* renderInfo) { vRenderInfos.push_back(renderInfo); }
		void SetRenderLayout(const RenderLayout* renderLayout) { mRenderLayout = renderLayout; }

#if  defined(_WIN32)
		void InitializeVulkan(HINSTANCE instance, HWND window);
#elif defined(__ANDROID__)
		void InitializeVulkan(ANativeWindow* window);
#endif 

		void InitializeRender(RenderPass* renderPass, DepthStencil* swapchainDepthStencil); //<--change to list and set swapchain to use user created images. Also move command pool to layout

		void BeginRenderPass(const DrawBuffer& drawBuffer); // Use renderinfo instead
		void EndRenderPass(const DrawBuffer& drawBuffer);
		void Submit();
		
		~Render();
	};
}