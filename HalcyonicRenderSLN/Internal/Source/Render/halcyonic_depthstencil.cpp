#include<precompiled.hpp>
#include<Render/halcyonic_render.hpp>
#include<Render/halcyonic_depthstencil.hpp>

using namespace hal;

hal::DepthStencil::DepthStencil(DepthStencilLayout * depthStencilLayout) : mDepthStencilLayout(depthStencilLayout)
{
	mMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mMemoryAllocateInfo.pNext = nullptr;
	mMemoryAllocateInfo.allocationSize = 0;
	mMemoryAllocateInfo.memoryTypeIndex = 0;

	VkMemoryRequirements memoryRequirements;
	HALCYONIC_VK_CHECK(vkCreateImage(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mDepthStencilLayout->GetImageCreateInfo(), nullptr, &mImage),"DepthStencil: Failed to create image.");
	vkGetImageMemoryRequirements(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mImage, &memoryRequirements);
	mMemoryAllocateInfo.allocationSize = memoryRequirements.size;
	mMemoryAllocateInfo.memoryTypeIndex = Render::Instance()->GetVulkanDevice().GetMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	HALCYONIC_VK_CHECK(vkAllocateMemory(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mMemoryAllocateInfo, nullptr, &mMemory), "DepthStencil: Failed to allocate image memory.");
	HALCYONIC_VK_CHECK(vkBindImageMemory(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mImage, mMemory, 0), "DepthStencil: Failed to bind image memory.");

	mDepthStencilLayout->SetViewCreateInfoImage(mImage);
	HALCYONIC_VK_CHECK(vkCreateImageView(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mDepthStencilLayout->GetViewCreateInfo(), nullptr, &mView), "DepthStencil: Failed to create image view.");
}
