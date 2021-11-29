#version 450

layout(location = 0) out vec4 outColor;

layout (location = 0) in VsOut {
	vec3 vertNormal;
	vec3 vertColor;
	vec4 worldPosition;
}vsOut;

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

vec3 calculateDirectionalLight (vec3 normal, vec3 viewDir, vec3 diffuseColor, float specularIntensity) {
	// vec4 clipPos = projectionMatrix * viewMatrix * vec4(vsOut.worldPos, 1.0);
	// float fragDepth = (clipPos.z/ clipPos.w) * 0.5 + 0.5;
	//vec3 diffuseColor = debug_color(fragDepth);
	float specularStrength = 0.1f;

	float Ka = 0.4f;
	float Ks = specularStrength;
	float Kd = 1.0f - Ks;

	vec3 lightDir = normalize(-frameUniforms.directionalLight.direction.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0),32);

	// float shadow = ShadowCalculation(fragDepth, normal, lightDir);

	float ambient  = Ka;
	float diffuse  = frameUniforms.directionalLight.intensity * diff * Kd;
	float specular = frameUniforms.directionalLight.intensity * spec * Ks;

	return (ambient + diffuse) * diffuseColor * frameUniforms.directionalLight.diffuse.xyz + 
	specular * frameUniforms.directionalLight.diffuse.xyz;
}


void main() {
	vec3 result = vec3(0);

	vec4 viewDir = frameUniforms.cameraPosition - vsOut.worldPosition;
	result += calculateDirectionalLight(vsOut.vertNormal, viewDir.xyz, vec3(0.5, 0.5, 0.5), 1.0f);
    outColor = vec4(result, 1.0);
}

