#pragma once

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Renderer/Vertex.h"
#include "Allocator.h"

class ResourceManager {
private:
	std::unordered_map<std::string, Mesh*> loadedMeshes;
	std::unordered_map<std::string, GraphicsPipeline> loadedPipelines;
	std::unordered_map<std::string, AllocatedImage> loadedTextures;

	StackAllocator* resourceAllocator;
public:
	ResourceManager();
	void release();

	Mesh* loadMesh(std::string path, int loaderFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	void savePipeline(std::string name, GraphicsPipeline);
	AllocatedImage* loadTexture(std::string path);

	GraphicsPipeline& getPipeline(std::string name);
};

#endif // !RESOURCEMANAGER_H
