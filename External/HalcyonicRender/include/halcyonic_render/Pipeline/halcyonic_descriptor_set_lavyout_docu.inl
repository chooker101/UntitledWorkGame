/*! \brief The Descriptor of the set of things you want to pass to each shader.

Usage is you pass in the shader stage where you want the descriptor. Then you pass which type of binding it is. Then lastly its bound location.
Example:
vertexshader.vert
	layout (binding = 0) uniform Block
	{
		mat4 projectionMatrix;
		mat4 modelMatrix;
		mat4 viewMatrix;
		mat4 normalMatrix;
	};
Then the input would be.
	DescriptorSet(ShaderStage::Vertex, LayoutBindingDescriptor::UniformBuffer, 0);
*/