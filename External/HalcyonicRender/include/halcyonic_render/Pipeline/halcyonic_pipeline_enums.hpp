#pragma once

namespace hal
{
	//!\enum enum class ShaderStage
	enum class ShaderStage
	{
		Vertex = VK_SHADER_STAGE_VERTEX_BIT,										//!<Vertex Shader
		Tessellation_Control = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,			//!<Tesselation Control Shader
		Tessellation_Evaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,		//!<Tessellation Evaluation Shader
		Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,									//!<Geometry Shader
		Pixel = VK_SHADER_STAGE_FRAGMENT_BIT,										//!<Fragment Shader
		Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,									//!<Fragment Shader
		Compute = VK_SHADER_STAGE_COMPUTE_BIT,										//!<Compute Shader
		All_Stages = VK_SHADER_STAGE_ALL_GRAPHICS,									//!<A Flag For All Stages
	};


	//!This is the Binding you want to use
	enum class LayoutBindingDescriptor
	{
		UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			//!<Uniform Buffer
		ImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER	//!<Image Sampler
	};
}