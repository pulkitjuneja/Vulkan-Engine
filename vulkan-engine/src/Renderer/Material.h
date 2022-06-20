#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "VulkanHelpers.h"
#include "VulkanContext.h"
#include <string>

struct Material {

	std::string name;
	GraphicsPipeline* pipeline;
	Texture* diffuseMap;
	bool needsUpdate;
	VkDescriptorSet MaterialDescriptorSet[MAX_FRAMES_IN_FLIGHT];

	Material();
	void setDiffuseTexture(Texture* diffuseMap);
};


#endif //MATERIAL_H