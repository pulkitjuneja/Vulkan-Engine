#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "VulkanHelpers.h"
#include <string>

struct Material {

	std::string name;
	GraphicsPipeline* pipeline;
	AllocatedImage* diffuseMap;

	Material() {
		diffuseMap = nullptr;
	}
};


#endif //MATERIAL_H