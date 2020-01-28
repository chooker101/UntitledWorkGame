#pragma once
#include <Pipeline/halcyonic_descriptor_layout.hpp>

namespace hal
{
	class DescriptorLayout;
	class Descriptor
	{
	protected:
		const DescriptorLayout* mDescriptorLayout;
		Descriptor(const DescriptorLayout* descriptorLayout) : mDescriptorLayout(descriptorLayout) {}
	public:
		LayoutBindingDescriptor GetLayoutBindingDescriptor() const { return mDescriptorLayout->GetLayoutBindingDescriptor(); }
		const DescriptorLayout* GetDescriptorLayout() const { return mDescriptorLayout; }
	};
}