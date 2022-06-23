#include "Material.h"
#include "EngineContext.h"
#include "VulkanContext.h"
#include "ResourceManagement/ResourceManager.h"

Material::Material() {
	diffuseMap = nullptr;
	needsUpdate = false;

	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();

	// TODO: this will be set by material loader
	pipeline = &EC::get()->resourceManager->getPipeline("BasePipeine");
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorSetAllocateInfo allocInfo = vk::getDescriptorAllocInfo(EC::get()->vulkanContext->descriptorPool, &pipeline->PerObjectLayout, 1);
		vkAllocateDescriptorSets(device, &allocInfo, &MaterialDescriptorSet[i]);
	}

	// Set defaul texture
	vk::Texture* default = EC::get()->resourceManager->getTexture("Assets/Textures/Default.jpg");
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler = default->sampler;
		imageBufferInfo.imageView = default->view;
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;

		write.dstBinding = 0;
		write.dstSet = MaterialDescriptorSet[i];
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &imageBufferInfo;
		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
	}
}

void Material::setDiffuseTexture(vk::Texture* diffuseMap)
{
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	this->diffuseMap = diffuseMap;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler = diffuseMap->sampler;
		imageBufferInfo.imageView = diffuseMap->view;
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;

		write.dstBinding = 0;
		write.dstSet = MaterialDescriptorSet[i];
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &imageBufferInfo;
		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
	}
}
