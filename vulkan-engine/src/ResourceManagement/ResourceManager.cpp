#include "ResourceManager.h"
#include "Logger.h"
#include "glm/glm.hpp"
#include "Renderer/VulkanContext.h"
#include "Allocator.h"
#include "stb_image.h"


ResourceManager::ResourceManager()
{
	resourceAllocator = Mem::Allocate<StackAllocator>(1024 * 1024 * 400, EC::get()->mainAllocator);  // 300 mb reserved for resources
}

Mesh* ResourceManager::loadMesh(std::string path, int loaderFlags)
{
	if (loadedMeshes.find(path) != loadedMeshes.end())
	{
		return loadedMeshes.find(path)->second;
	}

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, loaderFlags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::string error = import.GetErrorString();
		Logger::logError("ERROR::ASSIMP: " + error);
		return nullptr;
	}
	std::string directory = path.substr(0, path.find_last_of('/'));

	struct {
		bool operator () (aiMesh* a, aiMesh* b) {
			return a->mMaterialIndex < b->mMaterialIndex;
		}
	} meshMaterialSort;
	std::sort(scene->mMeshes, scene->mMeshes + scene->mNumMeshes, meshMaterialSort);

	int vertexCount = 0;
	int runningBaseIndexSum = 0;
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	bool hasNormals = true;
	bool hasTexCoords = true;
	bool hasTangents = true;
	bool hasBiTangents = true;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* currentMesh = scene->mMeshes[i];

		for (int j = 0; j < currentMesh->mNumVertices; j++)
		{
			Vertex vertex;
			glm::vec3 tangent;
			glm::vec3 biTangent;
			vertex.position = glm::vec3(currentMesh->mVertices[j].x, currentMesh->mVertices[j].y, currentMesh->mVertices[j].z);
			if (hasNormals && currentMesh->mNormals != nullptr) {
				vertex.normal = glm::vec3(currentMesh->mNormals[j].x, currentMesh->mNormals[j].y, currentMesh->mNormals[j].z);
			}
			vertex.color = glm::vec3(0.4f, 0.4f, 0.4f);
			//if (hasTexCoords && currentMesh->mTextureCoords[0]) {
			//	vertex.texCoords = glm::vec2(currentMesh->mTextureCoords[0][j].x, currentMesh->mTextureCoords[0][j].y);
			//}
			//if (hasTangents && currentMesh->mTangents != nullptr) {
			//	tangent = glm::vec3(currentMesh->mTangents[j].x, currentMesh->mTangents[j].y, currentMesh->mTangents[j].z);
			//}
			//if (hasBiTangents && currentMesh->mBitangents != nullptr) {
			//	biTangent = glm::vec3(currentMesh->mBitangents[j].x, currentMesh->mBitangents[j].y, currentMesh->mBitangents[j].z);
			//}
			vertices.push_back(vertex);
		}

		for (unsigned int j = 0; j < currentMesh->mNumFaces; j++)
		{
			aiFace face = currentMesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++)
				indices.push_back(face.mIndices[k]);
		}
		Mesh* newMesh = Mem::Allocate<Mesh>(resourceAllocator, std::move(vertices), std::move(indices));
		loadedMeshes.insert(make_pair(path, newMesh));
		Logger::logInfo("Mesh Loaded " + path);
		return loadedMeshes.find(path)->second;
	}

}

void ResourceManager::savePipeline(std::string name, GraphicsPipeline pipeline)
{
	if (loadedPipelines.find(name) != loadedPipelines.end())
	{
		Logger::logWarn("pipeline " + name + " already loaded");
		return;
	}

	loadedPipelines.insert(std::make_pair(name, pipeline));
}

AllocatedImage* ResourceManager::loadTexture(std::string path)
{
	auto& allocator = EC::get()->vulkanContext->allocator;

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels) {
		Logger::logError("Failed to load texture at path " + path);
		return nullptr;
	}

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

	AllocatedBuffer stagingBuffer; 
	VkBufferCreateInfo info = vkInit::getBufferCreateinfo(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	vmaCreateBuffer(allocator, &info, &vmaallocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, nullptr);

	void* data;
	vmaMapMemory(allocator, stagingBuffer.allocation, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vmaUnmapMemory(allocator, stagingBuffer.allocation);
	stbi_image_free(pixels);

	VkExtent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth = 1;

	VkImageCreateInfo imgInfo = vkInit::getImageCreateInfo(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);
	vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	AllocatedImage newImage{};

	if (vmaCreateImage(allocator, &imgInfo, &vmaallocInfo, &newImage.image, &newImage.allocation, nullptr)) {
		Logger::logError("Error creating image for texture at " + path);
	}
	VulkanContext* gc = EC::get()->vulkanContext;

	gc->immediateSubmit([&](VkCommandBuffer cmd) {
		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;
		VkImageMemoryBarrier imageBarrier_toTransfer = vkInit::getIMageMemoryBarrierInfo(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			newImage.image, range, VK_ACCESS_TRANSFER_WRITE_BIT);

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
			nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = vkInit::getBufferImageCopyInfo(VK_IMAGE_ASPECT_COLOR_BIT, imageExtent);
		vkCmdCopyBufferToImage(cmd, stagingBuffer.buffer, newImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = vkInit::getIMageMemoryBarrierInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			newImage.image, range, VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
			nullptr, 1, &imageBarrier_toReadable);
	});

	vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);
	Logger::logInfo("Texture Loaded " + path);
	loadedTextures.insert(std::make_pair(path, newImage));
	return &loadedTextures.find(path)->second;
}

GraphicsPipeline& ResourceManager::getPipeline(std::string name)
{
	if (loadedPipelines.find(name) == loadedPipelines.end()) {
		Logger::logError("Could not find existing pipeline " + name);
		return GraphicsPipeline{};
	}
	return loadedPipelines[name];
}

void ResourceManager::release()
{
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	auto& allocator = EC::get()->vulkanContext->allocator;

	for (auto pair : loadedTextures) {
		vmaDestroyImage(allocator, pair.second.image, pair.second.allocation);
	}

	for (auto pair : loadedMeshes) {
		pair.second->release();
	}

	for (auto pair : loadedPipelines) {
		vkDestroyPipeline(device, pair.second.pipeline, nullptr);
		vkDestroyPipelineLayout(device, pair.second.pipelineLayout, nullptr);
	}

	loadedMeshes.clear();
	loadedPipelines.clear();
}
