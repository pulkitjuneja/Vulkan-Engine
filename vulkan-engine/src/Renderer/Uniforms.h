#pragma once

#ifndef UNIFORMS_H
#define UNIFORMS_H

#include "glm/glm.hpp"

struct PerObjectUniforms {
	glm::vec4 data;
	glm::mat4 modelMatrix;
};

struct PerFrameUniforms {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};

#endif // !UNIFORMS_H
