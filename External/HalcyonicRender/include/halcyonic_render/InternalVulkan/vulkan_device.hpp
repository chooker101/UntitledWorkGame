#pragma once

namespace hal
{
	class VulkanDevice //Make a full class
	{
	private:
		friend class Render;
		struct QueueFamilyIndices
		{
			uint32_t graphics;
			uint32_t compute;
			uint32_t transfer;
		} mQueueFamilyIndices;
		VkDevice mLogicalDevice;
		VkPhysicalDevice mPhysicalDevice;
		VkPhysicalDeviceProperties mDeviceProperties;
		VkPhysicalDeviceFeatures mDeviceFeatures;
		VkPhysicalDeviceMemoryProperties mDeviceMemoryProperties;
		std::vector<VkQueueFamilyProperties> vQueueFamilyProperties;
	public:
		VulkanDevice(VkPhysicalDevice physicalDevice);

		VkResult CreateLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures, bool useSwapChain = true, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

		VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat) const;
		uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) const;
		uint32_t GetQueueFamiliyIndex(VkQueueFlagBits queueFlags) const;
		const VkPhysicalDevice& GetPhysicalDevice() const { return mPhysicalDevice; }
		const VkDevice& GetLogicalDevice() const { return mLogicalDevice; }
		const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const { return mDeviceFeatures; }
		const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return mDeviceProperties; }

		~VulkanDevice();
	};
}