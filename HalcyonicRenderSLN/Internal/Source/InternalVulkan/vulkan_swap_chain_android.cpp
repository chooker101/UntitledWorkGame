#include"vulkan_swap_chain.hpp"

using namespace hal;

void VulkanSwapChain::InitializeSurface(ANativeWindow* window) // todo seperate into different files
{
	VkResult err = VK_SUCCESS;

	VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.window = window;
	err = vkCreateAndroidSurfaceKHR(Application::Instance()->mVulkanInstance, &surfaceCreateInfo, NULL, &Application::Instance()->mSurface);
	assert(!err);

	uint32_t familyQueueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(Application::Instance()->mVulkanDevice->mPhysicalDevice, &familyQueueCount, nullptr);
	assert(familyQueueCount >= 1);

	std::vector<VkQueueFamilyProperties> queueProps(familyQueueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(Application::Instance()->mVulkanDevice->mPhysicalDevice, &familyQueueCount, queueProps.data());

	std::vector<VkBool32> supportsPresent(familyQueueCount);
	for (uint32_t i = 0; i < familyQueueCount; i++)
	{
		fpGetPhysicalDeviceSurfaceSupportKHR(Application::Instance()->mVulkanDevice->mPhysicalDevice, i, Application::Instance()->mSurface, &supportsPresent[i]);
	}

	uint32_t graphicsQueueNodeIndex = 0xffffffff;
	uint32_t presentQueueNodeIndex = 0xffffffff;

	for (uint32_t i = 0; i < familyQueueCount; i++)
	{
		if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			if (graphicsQueueNodeIndex == 0xffffffff)
			{
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE)
			{
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}

	if (presentQueueNodeIndex == 0xffffffff)
	{
		for (uint32_t i = 0; i < familyQueueCount; ++i)
		{
			if (supportsPresent[i] == VK_TRUE)
			{
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	assert(graphicsQueueNodeIndex != 0xffffffff || presentQueueNodeIndex != 0xffffffff);

	assert(graphicsQueueNodeIndex == presentQueueNodeIndex);

	mDeviceQueueIndex = graphicsQueueNodeIndex;

	uint32_t surfaceFormatCount;
	err = fpGetPhysicalDeviceSurfaceFormatsKHR(Application::Instance()->mVulkanDevice->mPhysicalDevice, Application::Instance()->mSurface, &surfaceFormatCount, NULL);
	assert(!err);
	assert(surfaceFormatCount > 0);

	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	err = fpGetPhysicalDeviceSurfaceFormatsKHR(Application::Instance()->mVulkanDevice->mPhysicalDevice, Application::Instance()->mSurface, &surfaceFormatCount, surfaceFormats.data());
	assert(!err);

	if ((surfaceFormatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		mColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		mColorFormat = surfaceFormats[0].format;
	}
	mColorSpace = surfaceFormats[0].colorSpace;
}

void hal::VulkanSwapChain::SetupFunctionPointers()
{
	GET_INSTANCE_PROC_ADDR(Application::Instance()->mVulkanInstance, GetPhysicalDeviceSurfaceSupportKHR);
	GET_INSTANCE_PROC_ADDR(Application::Instance()->mVulkanInstance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
	GET_INSTANCE_PROC_ADDR(Application::Instance()->mVulkanInstance, GetPhysicalDeviceSurfaceFormatsKHR);
	GET_INSTANCE_PROC_ADDR(Application::Instance()->mVulkanInstance, GetPhysicalDeviceSurfacePresentModesKHR);
	GET_DEVICE_PROC_ADDR(Application::Instance()->mVulkanDevice->mLogicalDevice, CreateSwapchainKHR);
	GET_DEVICE_PROC_ADDR(Application::Instance()->mVulkanDevice->mLogicalDevice, DestroySwapchainKHR);
	GET_DEVICE_PROC_ADDR(Application::Instance()->mVulkanDevice->mLogicalDevice, GetSwapchainImagesKHR);
	GET_DEVICE_PROC_ADDR(Application::Instance()->mVulkanDevice->mLogicalDevice, AcquireNextImageKHR);
	GET_DEVICE_PROC_ADDR(Application::Instance()->mVulkanDevice->mLogicalDevice, QueuePresentKHR);
}

void VulkanSwapChain::CreateSwapChain(uint32_t & width, uint32_t & height, const bool& vsync)
{
	VkResult err;
	VkSwapchainKHR oldSwapchain = mSwapChain;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(Application::Instance()->mVulkanDevice->mPhysicalDevice, Application::Instance()->mSurface, &surfaceCapabilities);
	assert(!err);

	uint32_t presentModeCount;
	err = fpGetPhysicalDeviceSurfacePresentModesKHR(Application::Instance()->mVulkanDevice->mPhysicalDevice, Application::Instance()->mSurface, &presentModeCount, NULL);
	assert(!err);
	assert(presentModeCount > 0);

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);

	err = fpGetPhysicalDeviceSurfacePresentModesKHR(Application::Instance()->mVulkanDevice->mPhysicalDevice, Application::Instance()->mSurface, &presentModeCount, presentModes.data());
	assert(!err);

	VkExtent2D swapchainExtent = {};

	if (surfaceCapabilities.currentExtent.width == (uint32_t)-1)
	{
		swapchainExtent.width = width;
		swapchainExtent.height = height;
	}
	else
	{
		swapchainExtent = surfaceCapabilities.currentExtent;
		width = surfaceCapabilities.currentExtent.width;
		height = surfaceCapabilities.currentExtent.height;
	}

	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	if (!vsync)
	{
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount + 1;
	if ((surfaceCapabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfaceCapabilities.maxImageCount))
	{
		desiredNumberOfSwapchainImages = surfaceCapabilities.maxImageCount;
	}

	VkSurfaceTransformFlagsKHR preTransform;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		// We prefer a non-rotated transform
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		preTransform = surfaceCapabilities.currentTransform;
	}

	VkSwapchainCreateInfoKHR swapchainCI = {};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = nullptr;
	swapchainCI.surface = Application::Instance()->mSurface;
	swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
	swapchainCI.imageFormat = mColorFormat;
	swapchainCI.imageColorSpace = mColorSpace;
	swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = nullptr;
	swapchainCI.presentMode = swapchainPresentMode;
	swapchainCI.oldSwapchain = oldSwapchain;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	err = fpCreateSwapchainKHR(Application::Instance()->mVulkanDevice->mLogicalDevice, &swapchainCI, nullptr, &mSwapChain);
	assert(!err);

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		for (uint32_t i = 0; i < mImageCount; i++)
		{
			vkDestroyImageView(Application::Instance()->mVulkanDevice->mLogicalDevice, v_SwapChainBuffers[i].view, nullptr);
		}
		fpDestroySwapchainKHR(Application::Instance()->mVulkanDevice->mLogicalDevice, oldSwapchain, nullptr);
	}

	err = fpGetSwapchainImagesKHR(Application::Instance()->mVulkanDevice->mLogicalDevice, mSwapChain, &mImageCount, nullptr);
	assert(!err);

	// Get the swap chain images
	v_Images.resize(mImageCount);
	err = fpGetSwapchainImagesKHR(Application::Instance()->mVulkanDevice->mLogicalDevice, mSwapChain, &mImageCount, v_Images.data());
	assert(!err);

	// Get the swap chain buffers containing the image and imageview
	v_SwapChainBuffers.resize(mImageCount);

	for (uint32_t i = 0; i < mImageCount; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = {};
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentView.pNext = nullptr;
		colorAttachmentView.format = mColorFormat;
		colorAttachmentView.components =
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentView.subresourceRange.baseMipLevel = 0;
		colorAttachmentView.subresourceRange.levelCount = 1;
		colorAttachmentView.subresourceRange.baseArrayLayer = 0;
		colorAttachmentView.subresourceRange.layerCount = 1;
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.flags = 0;

		v_SwapChainBuffers[i].image = v_Images[i];

		colorAttachmentView.image = v_SwapChainBuffers[i].image;

		err = vkCreateImageView(Application::Instance()->mVulkanDevice->mLogicalDevice, &colorAttachmentView, nullptr, &v_SwapChainBuffers[i].view);
		assert(!err);
	}
}

VkResult VulkanSwapChain::QueuePresentation(uint32_t imageIndex, VkSemaphore waitSemaphore)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &mSwapChain;
	presentInfo.pImageIndices = &imageIndex;

	if (waitSemaphore != VK_NULL_HANDLE)
	{
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
	}
	return fpQueuePresentKHR(Application::Instance()->mVulkanQueue, &presentInfo);
}

VulkanSwapChain::~VulkanSwapChain()
{
	if (mSwapChain != VK_NULL_HANDLE)
	{
		for (uint32_t i = 0; i < mImageCount; i++)
		{
			vkDestroyImageView(Application::Instance()->mVulkanDevice->mLogicalDevice, v_SwapChainBuffers[i].view, nullptr);
		}
	}
	if (Application::Instance()->mSurface != VK_NULL_HANDLE)
	{
		fpDestroySwapchainKHR(Application::Instance()->mVulkanDevice->mLogicalDevice, mSwapChain, nullptr);
		vkDestroySurfaceKHR(Application::Instance()->mVulkanInstance, Application::Instance()->mSurface, nullptr);
	}
	Application::Instance()->mSurface = VK_NULL_HANDLE;
	mSwapChain = VK_NULL_HANDLE;
}