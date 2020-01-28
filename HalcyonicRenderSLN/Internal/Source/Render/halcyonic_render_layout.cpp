#include <precompiled.hpp>
#include <Render/halcyonic_depthstencil.hpp>
#include <Render/halcyonic_renderpass.hpp>
#include <Render/halcyonic_framebuffer_layout.hpp>
#include <Render/halcyonic_render_layout.hpp>

using namespace hal;

hal::RenderLayout::RenderLayout(VkFormat colourFormat, VkFormat depthFormat, std::vector<VkClearValue> clearValues, uint32_t renderWidth, uint32_t renderHeight, bool enableVSync, bool enableValidation):
	mVulkanColourFormat(colourFormat),
	mVulkanDepthFormat(depthFormat),
	vClearValues(std::move(clearValues)),
	mRenderWidth(renderWidth),
	mRenderHeight(renderHeight),
	mEnableVSync(enableVSync),
	mEnableValidation(enableValidation),
	mSwapchainFramebufferLayout(new FramebufferLayout(renderWidth, renderHeight))
{
}
