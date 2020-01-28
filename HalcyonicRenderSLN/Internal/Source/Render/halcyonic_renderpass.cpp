#include<precompiled.hpp>
#include<Render/halcyonic_render.hpp>
#include<Render/halcyonic_renderpass_layout.hpp>
#include<Render/halcyonic_renderpass.hpp>

using namespace hal;

hal::RenderPass::RenderPass(const RenderPassLayout * renderPassLayout) : mRenderPassLayout(renderPassLayout)
{
	HALCYONIC_VK_CHECK(vkCreateRenderPass(Render::Instance()->GetVulkanDevice().GetLogicalDevice(), &mRenderPassLayout->GetRenderPassCI(), nullptr, &mRenderPass), "RenderPass: Could not create Vulkan Render Pass")
}

const VkRenderPass & hal::RenderPass::GetVulkanRenderPass() const
{
	return mRenderPass;
}
