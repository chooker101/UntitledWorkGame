#pragma once

namespace hal
{
	class RenderPassLayout;
	class RenderPass
	{
	private:
		const RenderPassLayout* mRenderPassLayout;
		VkRenderPass mRenderPass;
	public:
		RenderPass(const RenderPassLayout* renderPassLayout);
		const VkRenderPass& GetVulkanRenderPass() const;
	};
}