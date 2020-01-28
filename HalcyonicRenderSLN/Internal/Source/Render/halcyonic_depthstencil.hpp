#pragma once
#include<Render/halcyonic_depthstencil_layout.hpp>

namespace hal
{
	class DepthStencil
	{
	private:
		VkImage mImage;
		VkDeviceMemory mMemory;
		VkImageView mView;

		DepthStencilLayout* mDepthStencilLayout;
		VkMemoryAllocateInfo mMemoryAllocateInfo;
	public:
		DepthStencil(DepthStencilLayout* depthStencilLayout);
		const VkImageView& GetImageView() const { return mView; }
	};
}