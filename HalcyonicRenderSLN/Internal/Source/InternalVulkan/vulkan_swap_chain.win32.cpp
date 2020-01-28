#include"precompiled.hpp"
#include <Render/halcyonic_render.hpp>
#include"vulkan_swap_chain.hpp"


using namespace hal;

void hal::VulkanSwapChain::InitializeSurface(HINSTANCE platformHandle, HWND platformWindow) // todo seperate into different files
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = platformHandle;
	surfaceCreateInfo.hwnd = platformWindow;
	HALCYONIC_VK_CHECK(vkCreateWin32SurfaceKHR(hal::Render::Instance()->GetVulkanInstance(), &surfaceCreateInfo, nullptr, &hal::Render::Instance()->GetVulkanSurface()),"Create win surface failed");

	uint32_t familyQueueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), &familyQueueCount, nullptr);
	assert(familyQueueCount >= 1);

	std::vector<VkQueueFamilyProperties> queueProps(familyQueueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), &familyQueueCount, queueProps.data());

	std::vector<VkBool32> supportsPresent(familyQueueCount);
	for (uint32_t i = 0; i < familyQueueCount; i++)
	{
		fpGetPhysicalDeviceSurfaceSupportKHR(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), i, hal::Render::Instance()->GetVulkanSurface(), &supportsPresent[i]);
	}

	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;

	for (uint32_t i = 0; i < familyQueueCount; i++)
	{
		if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			if (graphicsQueueNodeIndex == UINT32_MAX)
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

	if (presentQueueNodeIndex == UINT32_MAX)
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
	assert(graphicsQueueNodeIndex != UINT32_MAX || presentQueueNodeIndex != UINT32_MAX);

	assert(graphicsQueueNodeIndex == presentQueueNodeIndex);

	mDeviceQueueIndex = graphicsQueueNodeIndex;

	uint32_t surfaceFormatCount = 0;
	HALCYONIC_VK_CHECK(fpGetPhysicalDeviceSurfaceFormatsKHR(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), hal::Render::Instance()->GetVulkanSurface(), &surfaceFormatCount, nullptr), "Get physical device surface formats count");

	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	HALCYONIC_VK_CHECK(fpGetPhysicalDeviceSurfaceFormatsKHR(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), hal::Render::Instance()->GetVulkanSurface(), &surfaceFormatCount, surfaceFormats.data()),"Get physical device surface formats");

	if ((surfaceFormatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		mColorFormat = Render::Instance()->GetColourFormat();
	}
	else
	{
		mColorFormat = surfaceFormats[0].format;
	}
	mColorSpace = surfaceFormats[0].colorSpace;
}

void hal::VulkanSwapChain::SetupFunctionPointers()
{
	GET_INSTANCE_PROC_ADDR(hal::Render::Instance()->GetVulkanInstance(), GetPhysicalDeviceSurfaceSupportKHR);
	GET_INSTANCE_PROC_ADDR(hal::Render::Instance()->GetVulkanInstance(), GetPhysicalDeviceSurfaceCapabilitiesKHR);
	GET_INSTANCE_PROC_ADDR(hal::Render::Instance()->GetVulkanInstance(), GetPhysicalDeviceSurfaceFormatsKHR);
	GET_INSTANCE_PROC_ADDR(hal::Render::Instance()->GetVulkanInstance(), GetPhysicalDeviceSurfacePresentModesKHR);
	GET_DEVICE_PROC_ADDR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), CreateSwapchainKHR);
	GET_DEVICE_PROC_ADDR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), DestroySwapchainKHR);
	GET_DEVICE_PROC_ADDR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), GetSwapchainImagesKHR);
	GET_DEVICE_PROC_ADDR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), AcquireNextImageKHR);
	GET_DEVICE_PROC_ADDR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), QueuePresentKHR);
}



void VulkanSwapChain::CreateSwapChain(const bool& vsync)
{
	VkResult err;
	VkSwapchainKHR oldSwapchain = mSwapChain;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), hal::Render::Instance()->GetVulkanSurface(), &surfaceCapabilities);
	assert(!err);

	uint32_t presentModeCount;
	err = fpGetPhysicalDeviceSurfacePresentModesKHR(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), hal::Render::Instance()->GetVulkanSurface(), &presentModeCount, nullptr);
	assert(!err);
	assert(presentModeCount > 0);

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);

	err = fpGetPhysicalDeviceSurfacePresentModesKHR(hal::Render::Instance()->GetVulkanDevice().GetPhysicalDevice(), hal::Render::Instance()->GetVulkanSurface(), &presentModeCount, presentModes.data());
	assert(!err);

	VkExtent2D swapchainExtent = {};

	swapchainExtent = surfaceCapabilities.currentExtent;

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
	swapchainCI.surface = hal::Render::Instance()->GetVulkanSurface();
	swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
	swapchainCI.imageFormat = mColorFormat;
	swapchainCI.imageColorSpace = mColorSpace;
	swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = nullptr;
	swapchainCI.presentMode = swapchainPresentMode;
	swapchainCI.oldSwapchain = oldSwapchain;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	err = fpCreateSwapchainKHR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &swapchainCI, nullptr, &mSwapChain);
	assert(!err);

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		for (uint32_t i = 0; i < mImageCount; i++)
		{
			vkDestroyImageView(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), vSwapChainBuffers[i].view, nullptr);
		}
		fpDestroySwapchainKHR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), oldSwapchain, nullptr);
	}

	err = fpGetSwapchainImagesKHR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mSwapChain, &mImageCount, nullptr);
	assert(!err);

	// Get the swap chain images
	vImages.resize(mImageCount);
	err = fpGetSwapchainImagesKHR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mSwapChain, &mImageCount, vImages.data());
	assert(!err);

	// Get the swap chain buffers containing the image and imageview
	vSwapChainBuffers.resize(mImageCount);

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

		vSwapChainBuffers[i].image = vImages[i];

		colorAttachmentView.image = vSwapChainBuffers[i].image;

		err = vkCreateImageView(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &colorAttachmentView, nullptr, &vSwapChainBuffers[i].view);
		assert(!err);
	}
}

VkResult VulkanSwapChain::GetNextImage(VkSemaphore presentCompleteSemaphore, uint32_t *imageIndex)
{
	return fpAcquireNextImageKHR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mSwapChain, 0xffffffffffffffff, presentCompleteSemaphore, (VkFence)nullptr, imageIndex);
}

VkResult VulkanSwapChain::QueuePresentation(uint32_t imageIndex, VkSemaphore waitSemaphore)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &mSwapChain;
	presentInfo.pImageIndices = &imageIndex;

	if (waitSemaphore != VK_NULL_HANDLE)
	{
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
	}
	return fpQueuePresentKHR(hal::Render::Instance()->GetVulkanQueue(), &presentInfo);
}

VulkanSwapChain::~VulkanSwapChain()
{
	if (mSwapChain != VK_NULL_HANDLE)
	{
		for (uint32_t i = 0; i < mImageCount; i++)
		{
			vkDestroyImageView(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), vSwapChainBuffers[i].view, nullptr);
		}
	}
	if (hal::Render::Instance()->GetVulkanSurface() != VK_NULL_HANDLE)
	{
		fpDestroySwapchainKHR(hal::Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mSwapChain, nullptr);
		vkDestroySurfaceKHR(hal::Render::Instance()->GetVulkanInstance(), hal::Render::Instance()->GetVulkanSurface(), nullptr);
	}
	hal::Render::Instance()->GetVulkanSurface() = VK_NULL_HANDLE;
	mSwapChain = VK_NULL_HANDLE;
}
