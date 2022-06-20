#pragma once

#ifndef  ENTITY_H
#define ENTITY_H

#include "Renderer/Vertex.h"
#include <string>
#include "Transform.h"
#include "Renderer/Pipelines/BasicPipeline.h"

class Entity {
protected:
	Mesh* mesh;
	std::string name;

public:
	Transform transform;

	Entity(std::string name, Mesh* mesh);
	Entity(std::string name);

	Mesh& getMesh() { return *mesh; }
	std::string getName() { return name; }
};

#endif // ! ENTITY_H
