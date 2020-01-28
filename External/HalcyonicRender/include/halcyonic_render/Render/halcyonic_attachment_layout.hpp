#pragma once

namespace hal
{
	class AttachmentLayout
	{
	protected:
		VkAttachmentDescription mAttachmentDescription = {};
		VkAttachmentReference mAttachmentReference = {};
	public:
		const VkAttachmentDescription* GetAttachmentDescription() const { return &mAttachmentDescription; }
		const VkAttachmentReference* GetAttachmentReference() const { return &mAttachmentReference; }

		void SetAttachmentLocation(uint32_t location) { mAttachmentReference.attachment = location; }
	};
}