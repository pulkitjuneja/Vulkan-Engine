#include "ResourceManager.h"
#include "Logger.h"
#include "glm/glm.hpp"
#include "Allocator.h"


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

ResourceManager::~ResourceManager()
{
	for (auto pair : loadedMeshes) {
		pair.second->release();
	}
	loadedMeshes.clear();

}
