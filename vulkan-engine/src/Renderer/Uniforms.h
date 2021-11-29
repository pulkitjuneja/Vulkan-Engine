#pragma once

#ifndef UNIFORMS_H
#define UNIFORMS_H

#include "glm/glm.hpp"

struct PerFrameUniforms {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};

struct PerObjectUniforms {
	glm::mat4 modelMatrix;
};

#endif // !UNIFORMS_H
