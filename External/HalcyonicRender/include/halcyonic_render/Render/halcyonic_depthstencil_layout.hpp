#pragma once
#include<Render/halcyonic_attachment_layout.hpp>

namespace hal
{
	class DepthStencilLayout : public AttachmentLayout
	{
	private:
		VkImageCreateInfo mImageCI = {};
		VkImageViewCreateInfo mImageViewCI = {};
	public:
		DepthStencilLayout();
		const VkImageCreateInfo& GetImageCreateInfo() const;
		const VkImageViewCreateInfo& GetViewCreateInfo() const;
		void SetViewCreateInfoImage(const VkImage& image);
	};
}