#include <precompiled.hpp>
#include <Pipeline/halcyonic_shader_input_layout.hpp>

using namespace hal;

ShaderInputLayout::ShaderInputLayout(std::vector<const InputAttributes*> inputAttributes, uint32_t inputStride, std::vector<const DescriptorLayout*> descriptors) :
	mInputAttributes(std::move(inputAttributes)),
	mInputStride(inputStride),
	mDescriptors(std::move(descriptors))
{
}

const InputAttributes * hal::ShaderInputLayout::GetInputAttribute(uint32_t index) const
{
	HALCYONIC_DEBUG((index < static_cast<uint32_t>(mInputAttributes.size())), "ShaderInputLayout: Input Attribute Index out of range.");
	return mInputAttributes[index];
}

uint32_t hal::ShaderInputLayout::GetInputAttributesSize() const
{
	return static_cast<uint32_t>(mInputAttributes.size());
}

uint32_t hal::ShaderInputLayout::GetInputStride() const
{
	return mInputStride;
}

const DescriptorLayout * hal::ShaderInputLayout::GetDescriptorSetLayout(uint32_t index) const
{
	HALCYONIC_DEBUG((index < mDescriptors.size()), "ShaderInputLayout: Input Attribute Index out of range.");
	return mDescriptors[index];
}

uint32_t hal::ShaderInputLayout::GetDescriptorSetLayoutsSize() const
{
	return static_cast<uint32_t>(mDescriptors.size());
}

void hal::ShaderInputLayout::SetStride(uint32_t stride)
{
	mInputStride = stride;
}

void hal::ShaderInputLayout::SetInputAttribute(const InputAttributes* input, uint32_t index)
{
	HALCYONIC_DEBUG((index < static_cast<uint32_t>(mInputAttributes.size())), "ShaderInputLayout: Input Attribute Index out of range. Try appending the Input Attribute.");
	if (mInputAttributes[index])
	{
		delete(mInputAttributes[index]);
	}
	mInputAttributes[index] = input;
}

void hal::ShaderInputLayout::AppendInputAttribute(const InputAttributes* input)
{
	mInputAttributes.push_back(input);
}

void hal::ShaderInputLayout::SetDescriptor(const DescriptorLayout* input, uint32_t index)
{
	HALCYONIC_DEBUG((index < static_cast<uint32_t>(mDescriptors.size())), "ShaderInputLayout: Descriptor Set Index out of range. Try appending the Input Attribute.");
	if (mDescriptors[index])
	{
		delete(mDescriptors[index]);
	}
	mDescriptors[index] = input;
}

void hal::ShaderInputLayout::AppendDescriptor(const DescriptorLayout* input)
{
	mDescriptors.push_back(input);
}
