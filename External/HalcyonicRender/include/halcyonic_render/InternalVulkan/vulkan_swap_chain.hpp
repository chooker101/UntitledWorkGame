#pragma once

namespace hal
{
	// Macro to get a procedure address based on a vulkan instance
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)															\
{																											\
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint));	\
	if (fp##entrypoint == nullptr)																				\
	{																										\
		exit(1);																							\
	}																										\
}

	// Macro to get a procedure address based on a vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)															\
{																										\
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));   \
	if (fp##entrypoint == nullptr)																			\
	{																									\
		exit(1);																						\
	}																									\
}
	class Render;

	class VulkanSwapChain
	{
	public:
		struct SwapChainBuffer
		{
			VkImage image;
			VkImageView view;
		};
	private:
		PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
		PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
		PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
		PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
		PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
		PFN_vkQueuePresentKHR fpQueuePresentKHR;

		VkFormat mColorFormat;
		VkColorSpaceKHR mColorSpace;
		VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;

		uint32_t mImageCount;
		uint32_t mDeviceQueueIndex = 0xffffffff;

		std::vector<VkImage> vImages;
		std::vector<SwapChainBuffer> vSwapChainBuffers;
	public:

#if defined (_WIN32)
		void InitializeSurface(HINSTANCE platformHandle, HWND platformWindow);
#elif defined (__ANDROID__)
		void InitializeSurface(ANativeWindow* window);
#endif

		void SetupFunctionPointers();

		void CreateSwapChain(const bool& vsync);

		SwapChainBuffer* GetSwapChainBuffer(uint32_t index) { return &vSwapChainBuffers[index]; }
		uint32_t GetImageCount() { return static_cast<uint32_t>(vImages.size()); }
		uint32_t GetQueueFamilyIndex() { return mDeviceQueueIndex; }

		VkResult GetNextImage(VkSemaphore presentCompleteSemaphore, uint32_t *imageIndex);

		VkResult QueuePresentation(uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

		~VulkanSwapChain();
	};
}