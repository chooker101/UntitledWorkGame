#include <precompiled.hpp>
#include <array>
#include <Render/halcyonic_depthstencil_layout.hpp>
#include <Render/halcyonic_renderpass_layout.hpp>

using namespace hal;

hal::RenderPassLayout::RenderPassLayout(std::vector<AttachmentLayout*> attachmentLayouts) : vAttachmentLayouts(std::move(attachmentLayouts))
{
	std::array<VkSubpassDependency, 2> dependencies;
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	for (uint32_t i = 0; i < vAttachmentLayouts.size(); ++i)
	{
		vAttachmentLayouts[i]->SetAttachmentLocation(i);

		switch(vAttachmentLayouts[i]->GetAttachmentReference()->layout)
		{
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			mColorAttachments.push_back(*vAttachmentLayouts[i]->GetAttachmentReference());
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
			mDepthAttachments.push_back(*vAttachmentLayouts[i]->GetAttachmentReference());
			break;
		default:
			HALCYONIC_DEBUG(false, "RenderPassLayout: Attachment type not recognized(Should be color or depth).");
		}

		mAttachmentDescriptions.push_back(*vAttachmentLayouts[i]->GetAttachmentDescription());
	}

	mSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	mSubpassDescription.colorAttachmentCount = 1;
	mSubpassDescription.pColorAttachments = mColorAttachments.data();
	mSubpassDescription.pDepthStencilAttachment = mDepthAttachments.data();
	mSubpassDescription.inputAttachmentCount = 0;
	mSubpassDescription.pInputAttachments = nullptr;
	mSubpassDescription.preserveAttachmentCount = 0;
	mSubpassDescription.pPreserveAttachments = nullptr;
	mSubpassDescription.pResolveAttachments = nullptr;

	mRenderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	mRenderPassCI.attachmentCount = static_cast<uint32_t>(mAttachmentDescriptions.size());
	mRenderPassCI.pAttachments = mAttachmentDescriptions.data();
	mRenderPassCI.subpassCount = 1;
	mRenderPassCI.pSubpasses = &mSubpassDescription;
	mRenderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
	mRenderPassCI.pDependencies = dependencies.data();
}

const VkRenderPassCreateInfo & hal::RenderPassLayout::GetRenderPassCI() const
{
	return mRenderPassCI;
}
