#pragma once
#include<Render/halcyonic_attachment_layout.hpp>

namespace hal
{
	class SwapchainColourPlaceholder : public AttachmentLayout
	{
	public:
		SwapchainColourPlaceholder(const VkFormat& colourFormat);
	};
}