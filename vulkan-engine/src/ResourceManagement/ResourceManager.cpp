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
	std::vector<SubMesh> submeshes;

	submeshes.resize(scene->mNumMaterials == 1 ? 1 : scene->mNumMaterials - 1);

	int currentSubmeshIndex = -1;
	int currentRunningMaterialIndex = -1;
	int indexOffset = 0;

	bool hasNormals = true;
	bool hasTexCoords = true;
	bool hasTangents = true;
	bool hasBiTangents = true;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* currentMesh = scene->mMeshes[i];

	if (currentMesh->mMaterialIndex != currentRunningMaterialIndex) {
			currentRunningMaterialIndex = currentMesh->mMaterialIndex;
			if (currentSubmeshIndex > -1) {
				runningBaseIndexSum += submeshes[currentSubmeshIndex].indexCount;
			}
			currentSubmeshIndex++;
			Material material;
			getAiSceneMaterial(scene, currentRunningMaterialIndex, directory, material);
			submeshes[currentSubmeshIndex].material = material;
			submeshes[currentSubmeshIndex].baseVertex = vertexCount;
			submeshes[currentSubmeshIndex].baseIndex = runningBaseIndexSum;
			indexOffset = 0;
		}
		submeshes[currentSubmeshIndex].indexCount += currentMesh->mNumFaces * 3;

		vertexCount += currentMesh->mNumVertices;

		for (int j = 0; j < currentMesh->mNumVertices; j++)
		{
			Vertex vertex;
			glm::vec3 tangent;
			glm::vec3 biTangent;
			vertex.position = glm::vec3(currentMesh->mVertices[j].x, currentMesh->mVertices[j].y, currentMesh->mVertices[j].z);
			if (hasNormals && currentMesh->mNormals != nullptr) {
				vertex.normal = glm::vec3(currentMesh->mNormals[j].x, currentMesh->mNormals[j].y, currentMesh->mNormals[j].z);
			}
			if (hasTexCoords && currentMesh->mTextureCoords[0]) {
				vertex.texCoords = glm::vec2(currentMesh->mTextureCoords[0][j].x, currentMesh->mTextureCoords[0][j].y);
			}
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
	}

	Mesh* newMesh = Mem::Allocate<Mesh>(resourceAllocator, std::move(vertices), std::move(indices), submeshes);
	loadedMeshes.insert(make_pair(path, newMesh));
	Logger::logInfo("Mesh Loaded " + path);
	return loadedMeshes.find(path)->second;

}

void ResourceManager::getAiSceneMaterial(const aiScene* scene, int materialIndex, std::string directory, Material& material)
{
	material.name = std::string(directory);
	material.name += "_";
	material.name += std::to_string(materialIndex);

	if (materialIndex >= 0)
	{
		material.pipeline = &getPipeline("BasePipeine");
		aiMaterial* aiMaterial = scene->mMaterials[materialIndex];
		Texture* diffuseMap = loadMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, directory);
		if (diffuseMap != NULL) {
			material.setDiffuseTexture(diffuseMap);
		}
		//material.specularMap = loadMaterialTexture(aiMaterial, aiTextureType_SPECULAR, directory);
		//material.normalMap = loadMaterialTexture(aiMaterial, aiTextureType_HEIGHT, directory);
	}

	else
	{	
		material.pipeline = &getPipeline("BasePipeine");
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

Texture* ResourceManager::loadTexture(std::string path)
{
	auto& allocator = EC::get()->vulkanContext->allocator;
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();

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

	Texture texture{};

	if (vmaCreateImage(allocator, &imgInfo, &vmaallocInfo, &texture.image.image, &texture.image.allocation, nullptr)) {
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
		VkImageMemoryBarrier imageBarrier_toTransfer = vkInit::getImageTransitionInfo(texture.image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range, 0, VK_ACCESS_TRANSFER_WRITE_BIT);

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = vkInit::getBufferImageCopyInfo(VK_IMAGE_ASPECT_COLOR_BIT, imageExtent);
		vkCmdCopyBufferToImage(cmd, stagingBuffer.buffer, texture.image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = vkInit::getImageTransitionInfo(texture.image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, range, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
			nullptr, 1, &imageBarrier_toReadable);
	});

	VkImageViewCreateInfo Viewinfo = vkInit::getImageViewCreateInfo(image_format, texture.image.image, VK_IMAGE_ASPECT_COLOR_BIT);
	vkCreateImageView(device, &Viewinfo, nullptr, &texture.view);
	VkSamplerCreateInfo samplerInfo = vkInit::getSamplerCreateInfo(VK_FILTER_NEAREST);
	vkCreateSampler(device, &samplerInfo, nullptr, &texture.sampler);

	vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);
	Logger::logInfo("Texture Loaded " + path);
	loadedTextures.insert(std::make_pair(path, texture));
	return &loadedTextures.find(path)->second;
}

Texture* ResourceManager::getTexture(std::string path)
{
	if (loadedTextures.find(path) == loadedTextures.end()) {
		Logger::logError("Texture" + path + "Not found");
		return NULL;
	}
	return &loadedTextures[path];
}

GraphicsPipeline& ResourceManager::getPipeline(std::string name)
{
	if (loadedPipelines.find(name) == loadedPipelines.end()) {
		Logger::logError("Could not find existing pipeline " + name);
		return GraphicsPipeline{};
	}
	return loadedPipelines[name];
}

Texture* ResourceManager::loadMaterialTexture(aiMaterial* aiMaterial, aiTextureType aiTextureType, std::string directory)
{
	aiString texturePath;
	if (aiMaterial->GetTextureCount(aiTextureType) > 0) {
		aiMaterial->GetTexture(aiTextureType, 0, &texturePath);
		//TextureType textureType = textureTypeMap[aiTextureType];
		std::string path = directory + "/" + texturePath.C_Str();
		Texture* tex = loadTexture(path);
		return tex;
	}
	else {
		return NULL;
	}
}

void ResourceManager::release()
{
	auto device = EC::get()->vulkanContext->getDevice().getLogicalDevice();
	auto& allocator = EC::get()->vulkanContext->allocator;

	for (auto pair : loadedTextures) {
		vmaDestroyImage(allocator, pair.second.image.image, pair.second.image.allocation);
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
