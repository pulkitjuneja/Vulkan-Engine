#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "VulkanHelpers.h"
#include "VulkanContext.h"
#include <string>

struct Material {

	std::string name;
	bool needsUpdate;

	vk::GraphicsPipeline* pipeline;
	vk::Texture* diffuseMap;
	vk::DescriptorSet materialDescriptorSet;

	Material() = default;

	// Call this function after setting the textures. Otherwise default textures will be set
	void create();

	//void setDiffuseTexture(vk::Texture* diffuseMap);
};


#endif //MATERIAL_H