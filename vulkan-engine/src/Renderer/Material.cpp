#include "Material.h"
#include "Core/EngineContext.h"
#include "VulkanContext.h"
#include "ResourceManagement/ResourceManager.h"

void Material::create() {

	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	vk::Texture* default = EC::get()->resourceManager->getTexture("Assets/Textures/Default.jpg");

	if (!diffuseMap) {
		diffuseMap = default;
	}

	if (!pipeline) {
		Logger::logError("No Pipeline set for material");
		return;
	}

	vk::DescriptorSetLayout& layout = pipeline->PerObjectLayout;
	layout.setTexture(0, diffuseMap);
	layout.getDescriptorSet(materialDescriptorSet);
}

//void Material::setDiffuseTexture(vk::Texture* diffuseMap)
//{
//	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
//	this->diffuseMap = diffuseMap;
//	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//		std::vector<vk::Descriptor> descriptors(1);
//		descriptors[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		descriptors[0].binding = 0;
//		descriptors[0].texture = diffuseMap;
//		MaterialDescriptorSet[i].update(descriptors);
//	}
//}
