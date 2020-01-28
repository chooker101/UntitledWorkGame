#pragma once

namespace hal
{
	class RenderPass;
	class FramebufferLayout;
	class DepthStencil;
	class RenderLayout
	{
	private:
		friend class Render;

		VkFormat mVulkanColourFormat = VK_FORMAT_R8G8B8A8_UNORM;
		mutable VkFormat mVulkanDepthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
		std::vector<VkClearValue> vClearValues = { {0.1f, 0.1f, 0.1f, 1.0f}, {1.0f, 0.0f} };

		uint32_t mRenderWidth = 1920;
		uint32_t mRenderHeight = 1080;

		bool mEnableVSync = false;
		bool mEnableValidation = false;

		FramebufferLayout* mSwapchainFramebufferLayout;
	public:
		RenderLayout(VkFormat colourFormat = VK_FORMAT_R8G8B8A8_UNORM, VkFormat depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT, std::vector<VkClearValue> clearValues = { {0.1f, 0.1f, 0.1f, 1.0f}, {1.0f, 0} }, uint32_t mRenderWidth = 1920, uint32_t mRenderHeight = 1080, bool enableVSync = false, bool enableValidation = false);
	};
}