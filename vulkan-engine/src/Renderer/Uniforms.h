#pragma once

#ifndef UNIFORMS_H
#define UNIFORMS_H

#include "glm/glm.hpp"
#include "Core/Constants.h"

struct Light {};

struct DirectionalLight : public Light {
public:
	glm::vec4 direction;
	glm::vec4 diffuse;
	ALIGNED(16) float intensity;
};

struct PerFrameUniforms {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec4 cameraPosition;
	ALIGNED(16) DirectionalLight directionalLight;
};

struct PerObjectUniforms {
	glm::mat4 modelMatrix;
};

#endif // !UNIFORMS_H
