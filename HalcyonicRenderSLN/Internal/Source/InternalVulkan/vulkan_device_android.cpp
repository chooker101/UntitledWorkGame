#include"vulkan_device.hpp"
#include"../Core/halcyonic_core.hpp"

hal::VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice)
{
	assert(physicalDevice);
	this->mPhysicalDevice = physicalDevice;

	vkGetPhysicalDeviceProperties(physicalDevice, &mDeviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &mDeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mDeviceMemoryProperties);
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	assert(queueFamilyCount > 0);
	v_QueueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, v_QueueFamilyProperties.data());
}

uint32_t hal::VulkanDevice::GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound)
{
	for (uint32_t i = 0; i < mDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((mDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		typeBits >>= 1;
	}
	if (memTypeFound)
	{
		*memTypeFound = false;
	}
	return 0;
}

uint32_t hal::VulkanDevice::GetQueueFamiliyIndex(VkQueueFlagBits queueFlags)
{
	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(v_QueueFamilyProperties.size()); i++)
		{
			if ((v_QueueFamilyProperties[i].queueFlags & queueFlags) && ((v_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	if (queueFlags & VK_QUEUE_TRANSFER_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(v_QueueFamilyProperties.size()); i++)
		{
			if ((v_QueueFamilyProperties[i].queueFlags & queueFlags) && ((v_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((v_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < static_cast<uint32_t>(v_QueueFamilyProperties.size()); i++)
	{
		if (v_QueueFamilyProperties[i].queueFlags & queueFlags)
		{
			return i;
			break;
		}
	}
}

VkResult hal::VulkanDevice::CreateLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures, bool useSwapChain, VkQueueFlags requestedQueueTypes)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	// Get queue family indices for the requested queue family types
	// Note that the indices may overlap depending on the implementation

	const float defaultQueuePriority(0.0f);

	// Graphics queue
	if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
	{
		mQueueFamilyIndices.graphics = GetQueueFamiliyIndex(VK_QUEUE_GRAPHICS_BIT);
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = mQueueFamilyIndices.graphics;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back(queueInfo);
	}
	else
	{
		mQueueFamilyIndices.graphics = VK_NULL_HANDLE;
	}

	// Dedicated compute queue
	if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
	{
		mQueueFamilyIndices.compute = GetQueueFamiliyIndex(VK_QUEUE_COMPUTE_BIT);
		if (mQueueFamilyIndices.compute != mQueueFamilyIndices.graphics)
		{
			// If compute family index differs, we need an additional queue create info for the compute queue
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = mQueueFamilyIndices.compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else
	{
		// Else we use the same queue
		mQueueFamilyIndices.compute = mQueueFamilyIndices.graphics;
	}

	// Dedicated transfer queue
	if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
	{
		mQueueFamilyIndices.transfer = GetQueueFamiliyIndex(VK_QUEUE_TRANSFER_BIT);
		if ((mQueueFamilyIndices.transfer != mQueueFamilyIndices.graphics) && (mQueueFamilyIndices.transfer != mQueueFamilyIndices.compute))
		{
			// If compute family index differs, we need an additional queue create info for the compute queue
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = mQueueFamilyIndices.transfer;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else
	{
		// Else we use the same queue
		mQueueFamilyIndices.transfer = mQueueFamilyIndices.graphics;
	}

	// Create the logical device representation
	std::vector<const char*> deviceExtensions;
	if (useSwapChain)
	{
		// If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	//deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
	//enableDebugMarkers = true;

	if (deviceExtensions.size() > 0)
	{
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	Core::CheckVulkanResults(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mLogicalDevice));

	return VK_SUCCESS;
}

hal::VulkanDevice::~VulkanDevice()
{
	if (mLogicalDevice != nullptr)
	{
		vkDestroyDevice(mLogicalDevice, nullptr);
	}
}