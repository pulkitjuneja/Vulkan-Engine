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
	StackAllocator* resourceAllocator;
public:
	ResourceManager();
	Mesh* loadMesh(std::string path, int loaderFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	~ResourceManager();
};

#endif // !RESOURCEMANAGER_H
