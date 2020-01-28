#pragma once

#include <Pipeline/halcyonic_input_attributes.hpp>
#include <Pipeline/halcyonic_descriptor_layout.hpp>

namespace hal
{ 
	class ShaderInputLayout
	{
	private:
		uint32_t mInputStride;
		std::vector<const InputAttributes*> mInputAttributes;
		std::vector<const DescriptorLayout*> mDescriptors;
	public:
		//Creates Shader Layout InputAttribute order is binding order. Stride is size of your vertex.
		ShaderInputLayout(std::vector<const InputAttributes*> inputAttributes, uint32_t inputStride, std::vector<const DescriptorLayout*> descriptors);

		const InputAttributes* GetInputAttribute(uint32_t index) const;
		uint32_t GetInputAttributesSize() const;
		uint32_t GetInputStride() const;
		const DescriptorLayout* GetDescriptorSetLayout(uint32_t index) const;
		uint32_t GetDescriptorSetLayoutsSize() const;

		void SetStride(uint32_t stride);
		void SetInputAttribute(const InputAttributes * input, uint32_t index);
		void AppendInputAttribute(const InputAttributes * input);
		void SetDescriptor(const DescriptorLayout* input, uint32_t index);
		void AppendDescriptor(const DescriptorLayout* input);
	};
}