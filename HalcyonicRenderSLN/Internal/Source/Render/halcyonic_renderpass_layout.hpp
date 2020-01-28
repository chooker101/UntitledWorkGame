#pragma once

namespace hal
{
	class AttachmentLayout;
	class RenderPassLayout
	{
	private:
		std::vector<AttachmentLayout*> vAttachmentLayouts;
		std::vector<VkAttachmentReference> mColorAttachments;
		std::vector<VkAttachmentReference> mDepthAttachments;
		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		VkSubpassDescription mSubpassDescription = {};
		VkRenderPassCreateInfo mRenderPassCI = {};
	public:
		RenderPassLayout(std::vector<AttachmentLayout*> attachmentLayouts);
		const VkRenderPassCreateInfo& GetRenderPassCI() const;
	};
}