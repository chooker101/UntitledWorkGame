#include <precompiled.hpp>
#include <InternalVulkan/vulkan_swap_chain.hpp>
#include <Command/halcyonic_command_pool.hpp>
#include <Command/halcyonic_setup_command_buffer.hpp>
#include <DrawInfo/halcyonic_draw_buffer.hpp>
#include <Render/halcyonic_depthstencil.hpp>
#include <Render/halcyonic_renderpass.hpp>
#include <Render/halcyonic_framebuffer_layout.hpp>
#include <Render/halcyonic_render_layout.hpp>
#include <Render/halcyonic_render_info.hpp>
#include <Render/halcyonic_render.hpp>

#include <array>
#include <fstream>

using namespace hal;

render_ptr hal::Render::s_Instance = nullptr;

hal::Render::Render() : mVulkanInstance(VK_NULL_HANDLE), mVulkanQueue(VK_NULL_HANDLE), mSurface(VK_NULL_HANDLE), mVulkanDevice(nullptr), mSwapChain(new VulkanSwapChain())
{
}

void hal::Render::SetupFrameBuffer()
{
	vFrameBuffers.resize(mSwapChain->GetImageCount());
	mRenderLayout->mSwapchainFramebufferLayout->AppendViewAttachment(mSwapChain->GetSwapChainBuffer(0)->view);
	mRenderLayout->mSwapchainFramebufferLayout->AppendViewAttachment(mSwapchainDepthStencil->GetImageView());
	mRenderLayout->mSwapchainFramebufferLayout->SetRenderPass(*mRenderPass);

	HALCYONIC_VK_CHECK(vkCreateFramebuffer(mVulkanDevice->GetLogicalDevice(), &mRenderLayout->mSwapchainFramebufferLayout->GetFrameBufferCI(), nullptr, &vFrameBuffers[0]), "Render: Create frame buffer failed");

	for (uint32_t i = 1; i < vFrameBuffers.size(); i++)
	{
		mRenderLayout->mSwapchainFramebufferLayout->InsertViewAttachment(mSwapChain->GetSwapChainBuffer(i)->view, 0);

		HALCYONIC_VK_CHECK(vkCreateFramebuffer(mVulkanDevice->GetLogicalDevice(), &mRenderLayout->mSwapchainFramebufferLayout->GetFrameBufferCI(), nullptr, &vFrameBuffers[i]), "Render: Create frame buffer failed");
	}
}

void hal::Render::PrepareSychronizationFences()
{
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vFences.resize(mSwapChain->GetImageCount());
	for (auto& fence : vFences)
	{
		HALCYONIC_VK_CHECK(vkCreateFence(mVulkanDevice->GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence), "Render: Create fence failed");
	}
}

VkResult hal::Render::CreateVulkanInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Halcyonic";
	appInfo.pEngineName = "Halcyonic";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	if (enabledExtensions.size() > 0)
	{
		if (mRenderLayout->mEnableValidation)
		{
			enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
	}
	if (mRenderLayout->mEnableValidation)
	{
		//instanceCreateInfo.enabledLayerCount = vkDebug::validationLayerCount;
		//instanceCreateInfo.ppEnabledLayerNames = vkDebug::validationLayerNames;
	}
	return vkCreateInstance(&instanceCreateInfo, nullptr, &mVulkanInstance);
}

void hal::Render::SetupDefaultSemaphores()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;
	
	HALCYONIC_VK_CHECK(vkCreateSemaphore(mVulkanDevice->mLogicalDevice, &semaphoreCreateInfo, nullptr, &mRenderCompleted), "Render: Could not create default semaphore");
	HALCYONIC_VK_CHECK(vkCreateSemaphore(mVulkanDevice->mLogicalDevice, &semaphoreCreateInfo, nullptr, &mPresentCompleted), "Render: Could not create default semaphore");
}

void hal::Render::CreateInstance()
{
	if (s_Instance == nullptr)
	{
		s_Instance = static_cast<render_ptr>(new hal::Render());
	}
}

const render_ptr& hal::Render::Instance()
{
	HALCYONIC_DEBUG((s_Instance != nullptr),"Render: Render static instance not created. Call hal::Render::CreateInstance() before hal::Render::Instance().")
	return s_Instance;
}

void hal::Render::DestroyInstance()
{
	s_Instance.release();
	s_Instance = nullptr;
}

void hal::Render::InitializeVulkan(HINSTANCE instance, HWND window)
{
	VkResult err = CreateVulkanInstance();

	assert(err == VK_SUCCESS);
	uint32_t gpuCount = 0;

	HALCYONIC_VK_CHECK(vkEnumeratePhysicalDevices(mVulkanInstance, &gpuCount, nullptr),"Render: Enumerate physical devices failed");
	assert(gpuCount > 0);

	std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
	err = vkEnumeratePhysicalDevices(mVulkanInstance, &gpuCount, physicalDevices.data());
	assert(err == VK_SUCCESS);

	mVulkanDevice = new VulkanDevice(physicalDevices[0]);

	vkGetPhysicalDeviceFeatures(mVulkanDevice->GetPhysicalDevice(), &mVulkanDevice->mDeviceFeatures);
	HALCYONIC_VK_CHECK(mVulkanDevice->CreateLogicalDevice(mVulkanDevice->mDeviceFeatures),"Render: Create logical device failed");

	vkGetPhysicalDeviceProperties(mVulkanDevice->GetPhysicalDevice(), &mVulkanDevice->mDeviceProperties);


	vkGetPhysicalDeviceMemoryProperties(mVulkanDevice->GetPhysicalDevice(), &mVulkanDevice->mDeviceMemoryProperties);


	vkGetDeviceQueue(mVulkanDevice->GetLogicalDevice(), mVulkanDevice->mQueueFamilyIndices.graphics, 0, &mVulkanQueue);

	VkBool32 validDepthFormat = mVulkanDevice->GetSupportedDepthFormat(mVulkanDevice->GetPhysicalDevice(), &mRenderLayout->mVulkanDepthFormat); //<--Dirty
	HALCYONIC_DEBUG(validDepthFormat, "Render: No valid depth format");

	Render::Instance()->mSwapChain->SetupFunctionPointers();

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = mVulkanDevice->GetPhysicalDevice();
	allocatorInfo.device = mVulkanDevice->GetLogicalDevice();

	HALCYONIC_VK_CHECK(vmaCreateAllocator(&allocatorInfo, &mAllocator), "Render: Could not create a memory allocator");

	SetupDefaultSemaphores();

	mSwapChain->InitializeSurface(instance, window);
}

VkImage * hal::Render::GetCurrentImage()
{
	return &mSwapChain->GetSwapChainBuffer(mCurrentFrame)->image;
}

void hal::Render::InitializeRender(RenderPass* renderPass, DepthStencil* swapchainDepthStencil)
{
	mRenderPass = renderPass;
	mSwapchainDepthStencil = swapchainDepthStencil;

	//Create swapchian
	mSwapChain->CreateSwapChain(false); //todo set up vsync setting also requires setup buffer add har requirements

	//Setupt the frame buffer
	SetupFrameBuffer();

	PrepareSychronizationFences();

	isRunning = true;
}

void hal::Render::BeginRenderPass(const DrawBuffer& drawBuffer)
{
	HALCYONIC_VK_CHECK(mSwapChain->GetNextImage(mPresentCompleted, &mCurrentFrame), "Render: Could not get next swapchain image");

	mRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; //move to happen once
	mRenderPassBeginInfo.pNext = nullptr;
	mRenderPassBeginInfo.renderPass = mRenderPass->GetVulkanRenderPass();
	mRenderPassBeginInfo.renderArea.offset.x = 0;
	mRenderPassBeginInfo.renderArea.offset.y = 0;
	mRenderPassBeginInfo.renderArea.extent.width = mRenderLayout->mRenderWidth;
	mRenderPassBeginInfo.renderArea.extent.height = mRenderLayout->mRenderHeight;
	mRenderPassBeginInfo.clearValueCount = 2;
	mRenderPassBeginInfo.pClearValues = mRenderLayout->vClearValues.data();
	mRenderPassBeginInfo.framebuffer = vFrameBuffers[mCurrentFrame];

	vkCmdBeginRenderPass(drawBuffer.GetCommandBuffer(), &mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	mVulkanViewport.width = (float)mRenderLayout->mRenderWidth;
	mVulkanViewport.height = (float)mRenderLayout->mRenderHeight;
	mVulkanViewport.minDepth = 0.0f;
	mVulkanViewport.maxDepth = 1.0f;

	vkCmdSetViewport(drawBuffer.GetCommandBuffer(), 0, 1, &mVulkanViewport);

	mDynamicScissorState.extent.width = mRenderLayout->mRenderWidth;
	mDynamicScissorState.extent.height = mRenderLayout->mRenderHeight;
	mDynamicScissorState.offset.x = 0;
	mDynamicScissorState.offset.y = 0;

	vkCmdSetScissor(drawBuffer.GetCommandBuffer(), 0, 1, &mDynamicScissorState);
}

void hal::Render::EndRenderPass(const DrawBuffer& drawBuffer)
{
	vkCmdEndRenderPass(drawBuffer.GetCommandBuffer());
}

void hal::Render::Submit()
{
	if (isRunning)
	{
		// Use a fence to wait until the command buffer has finished execution before using it again
		HALCYONIC_VK_CHECK(vkWaitForFences(mVulkanDevice->GetLogicalDevice(), 1, &vFences[mCurrentFrame], VK_TRUE, UINT64_MAX), "Render: Wait For Fences in Render");
		HALCYONIC_VK_CHECK(vkResetFences(mVulkanDevice->GetLogicalDevice(), 1, &vFences[mCurrentFrame]), "Render: Reset Fences in Render");

		// Pipeline stage at which the queue submission will wait
		
		HALCYONIC_DEBUG((vRenderInfos.size() > 0), "Render: No RenderInfos set to draw");
		
		for (auto& renderInfo : vRenderInfos)
		{
			HALCYONIC_VK_CHECK(vkQueueSubmit(mVulkanQueue, 1, &renderInfo->GetSubmitInfo(), vFences[mCurrentFrame]), "Render: Queue Submit failed");
			HALCYONIC_VK_CHECK(vkQueueWaitIdle(mVulkanQueue), "Render: Queue Wait Idle failed");
		}
		
		// Present the current buffer to the swap chain
		// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
		// This ensures that the image is not presented to the windowing system until all commands have been submitted
		HALCYONIC_VK_CHECK(mSwapChain->QueuePresentation(mCurrentFrame, mRenderCompleted),"Render: Queue Presentation Failed.");
	}
}

hal::Render::~Render()
{
	s_Instance.release();
}
