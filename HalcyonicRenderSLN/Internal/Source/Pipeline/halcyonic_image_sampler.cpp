#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Command/halcyonic_setup_command_buffer.hpp>
#include <Buffer/halcyonic_buffer.hpp>
#include <Pipeline/halcyonic_image_sampler.hpp>

using namespace hal;

void hal::ImageSampler::SetImageLayout(VkCommandBuffer cmdBuffer, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange)
{
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = NULL;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = mImage;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:

		imageMemoryBarrier.srcAccessMask = 0;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:

		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:

		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	}

	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:

		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:

		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:

		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	}

	VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(cmdBuffer, srcStageFlags, destStageFlags, 0, 0, nullptr,0, nullptr, 1, &imageMemoryBarrier);
}

hal::ImageSampler::ImageSampler(const SetupCommandBuffer& setupCommandBuffer, uint32_t size, uint8_t * data, ImageSamplerLayout * imageSamplerLayout) : mImageSamplerLayout(imageSamplerLayout)
{
	Buffer stagingBuffer = Buffer(size, data, BufferType::TransferBuffer, VK_SHARING_MODE_EXCLUSIVE);

	std::vector<VkBufferImageCopy> bufferCopyRegions;
	uint32_t offset = 0;
	uint32_t mipLevels = imageSamplerLayout->GetMipLevels();

	for (uint32_t i = 0; i < mipLevels; i++)
	{
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = i;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = imageSamplerLayout->GetWidth();
		bufferCopyRegion.imageExtent.height = imageSamplerLayout->GetHeight();
		bufferCopyRegion.imageExtent.depth = 1;
		bufferCopyRegion.bufferOffset = offset;

		bufferCopyRegions.push_back(bufferCopyRegion);

		offset += size;
	}

	VkMemoryRequirements memReqs = {};
	VkMemoryAllocateInfo memAllocInfo = {};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	HALCYONIC_VK_CHECK(vkCreateImage(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mImageSamplerLayout->GetImageCI(), nullptr, &mImage), "ImageSampler: Failed to create image");

	vkGetImageMemoryRequirements(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mImage, &memReqs);

	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = Render::Instance()->GetVulkanDevice().GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	HALCYONIC_VK_CHECK(vkAllocateMemory(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &memAllocInfo, nullptr, &mDeviceMemory), "ImageSampler: Failed to allocate image memory"); // Move to vma In shared area
	HALCYONIC_VK_CHECK(vkBindImageMemory(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), mImage, mDeviceMemory, 0), "ImageSampler: Failed to bind image memory");

	VkImageSubresourceRange subresourceRange = {}; //maybe move to member list and remove param 
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = mipLevels;
	subresourceRange.layerCount = 1;

	SetImageLayout(setupCommandBuffer.GetVulkanCommandBuffer(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

	vkCmdCopyBufferToImage(setupCommandBuffer.GetVulkanCommandBuffer(), *stagingBuffer.GetVkBuffer(), mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

	mImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	SetImageLayout(setupCommandBuffer.GetVulkanCommandBuffer(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mImageLayout, subresourceRange);

	HALCYONIC_VK_CHECK(vkCreateSampler(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mImageSamplerLayout->GetSamplerCI(), nullptr, &mSampler), "ImageSampler: Failed to create sampler");

	mImageSamplerLayout->SetImage(mImage);
	HALCYONIC_VK_CHECK(vkCreateImageView(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mImageSamplerLayout->GetImageViewCI(), nullptr, &mImageView), "ImageSampler: Failed to create image view");
}
