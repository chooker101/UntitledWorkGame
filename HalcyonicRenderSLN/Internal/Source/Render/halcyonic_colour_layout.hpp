#pragma once
#include<Render/halcyonic_attachment_layout.hpp>

namespace hal
{
	class ColourLayout : public AttachmentLayout //Fix and move layouts to own folder
	{
	private:
		VkImageCreateInfo mImageCI = {};
		VkImageViewCreateInfo mImageViewCI = {};
	public:
		ColourLayout(const VkFormat& colourFormat); //Needs fixing
		const VkImageCreateInfo& GetImageCreateInfo() const;
		const VkImageViewCreateInfo& GetViewCreateInfo() const;
		void SetViewCreateInfoImage(const VkImage& image);
	};
}