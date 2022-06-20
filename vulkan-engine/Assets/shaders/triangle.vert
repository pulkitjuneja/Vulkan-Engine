#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

layout (location = 0) out VsOut {
	vec3 vertNormal;
	vec2 texCoords;
	vec4 worldPosition;
}vsOut;

// uniform declarations
struct DirectionalLight {
	vec4 direction;
	vec4 diffuse;
	float intensity;
};

layout(set = 0, binding = 0) uniform perFrameUniforms{
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec4 cameraPosition;
	DirectionalLight directionalLight;
} frameUniforms;

struct PerObjectUniforms{
	mat4 modelMatrix;
};

layout(std140, set = 0, binding = 1) readonly buffer ObjectBuffer{
	PerObjectUniforms objects[];
} objectBuffer;

void main()
{
	mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].modelMatrix;
	vsOut.worldPosition = modelMatrix * vec4(vPosition, 1.0f);
	gl_Position = frameUniforms.projectionMatrix * frameUniforms.viewMatrix * vsOut.worldPosition;
	vsOut.texCoords = vTexCoords;
	vsOut.vertNormal = mat3(transpose(inverse(modelMatrix))) * vNormal;
}