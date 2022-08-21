#include "Material.h"
#include "EngineContext.h"
#include "VulkanContext.h"
#include "ResourceManagement/ResourceManager.h"

Material::Material() {
	diffuseMap = nullptr;
	needsUpdate = false;

	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	vk::Texture* default = EC::get()->resourceManager->getTexture("Assets/Textures/Default.jpg");

	// TODO: this will be set by material loader
	pipeline = &EC::get()->resourceManager->getPipeline("BasePipeine");
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			
		pipeline->PerObjectLayout.setTexture(0, default);
		pipeline->PerObjectLayout.getDescriptorSet(MaterialDescriptorSet[i]);
	}
}

void Material::setDiffuseTexture(vk::Texture* diffuseMap)
{
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	this->diffuseMap = diffuseMap;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		std::vector<vk::Descriptor> descriptors(1);
		descriptors[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptors[0].binding = 0;
		descriptors[0].texture = diffuseMap;
		MaterialDescriptorSet[i].update(descriptors);
	}
}
