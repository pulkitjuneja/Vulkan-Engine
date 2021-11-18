#pragma once

#ifndef UNIFORMS_H
#define UNIFORMS_H

#include "glm/glm.hpp"

struct PerObjectUniforms {
	glm::vec4 data;
	glm::mat4 modelMatrix;
};

#endif // !UNIFORMS_H
