#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

layout (binding = 0) uniform Block 
{
	mat4 modelMatrix;
	mat4 viewMatrix;
    mat4 projectionMatrix;
};

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main() 
{
	outColor = inColor;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
}