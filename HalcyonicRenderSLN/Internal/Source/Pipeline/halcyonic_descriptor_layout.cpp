#include <precompiled.hpp>
#include <Pipeline/halcyonic_shader_input_layout.hpp>
#include <Pipeline/halcyonic_descriptor_layout.hpp>

using namespace hal;

hal::DescriptorLayout::DescriptorLayout(ShaderStage shaderStage, LayoutBindingDescriptor layoutBinding, uint32_t bindingLocation):
	mShaderStage(shaderStage),
	mLayoutBinding(layoutBinding),
	mBindingLocation(bindingLocation)
{
}

ShaderStage hal::DescriptorLayout::GetShaderStage() const
{
	return mShaderStage;
}

LayoutBindingDescriptor hal::DescriptorLayout::GetLayoutBindingDescriptor() const
{
	return mLayoutBinding;
}

uint32_t hal::DescriptorLayout::GetBindingLocation() const
{
	return mBindingLocation;
}

void hal::DescriptorLayout::SetShaderStage(ShaderStage shaderStage)
{
	mShaderStage = shaderStage;
}

void hal::DescriptorLayout::SetLayoutBindingDescriptor(LayoutBindingDescriptor layoutBinding)
{
	mLayoutBinding = layoutBinding;
}

void hal::DescriptorLayout::SetBindingLocation(uint32_t bindingLocation)
{
	mBindingLocation = bindingLocation;
}
