#pragma once
#include <Pipeline/halcyonic_pipeline_enums.hpp>

namespace hal
{
	//https://halcyonicrenderdocs.000webhostapp.com/classhal_1_1_descriptor_set.html
//#include "halcyonic_descriptor_set_layout_docu.inl"
	class DescriptorSetLayout //Rename to descriptorlayout
	{
	private:
		ShaderStage mShaderStage;
		LayoutBindingDescriptor mLayoutBinding;
		uint32_t mBindingLocation;
	public:
		DescriptorSetLayout(ShaderStage shaderStage, LayoutBindingDescriptor layoutBinding, uint32_t bindingLocation);

		ShaderStage GetShaderStage() const;
		LayoutBindingDescriptor GetLayoutBindingDescriptor() const;
		uint32_t GetBindingLocation() const;

		void SetShaderStage(ShaderStage shaderStage);
		void SetLayoutBindingDescriptor(LayoutBindingDescriptor layoutBinding);
		void SetBindingLocation(uint32_t bindingLocation);
	};
}