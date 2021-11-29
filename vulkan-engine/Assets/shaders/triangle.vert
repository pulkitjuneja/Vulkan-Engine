#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform perFrameUniforms{
	mat4 viewMatrix;
	mat4 projectionMatrix;
} frameUniforms;

struct PerObjectUniforms{
	mat4 modelMatrix;
};

layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{

	PerObjectUniforms objects[];
} objectBuffer;


layout( push_constant ) uniform constants
{
	vec4 data;
	mat4 modelMatrix;
} objectUniforms;

void main()
{
	mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].modelMatrix;
	gl_Position = frameUniforms.projectionMatrix * frameUniforms.viewMatrix * modelMatrix * vec4(vPosition, 1.0f);
	fragColor = vNormal;
}