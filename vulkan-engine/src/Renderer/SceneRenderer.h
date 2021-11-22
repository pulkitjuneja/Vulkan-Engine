#pragma once

#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include "Scene/Scene.h"
#include "Uniforms.h"
#include "EngineContext.h"
#include "VulkanContext.h"

class SceneRenderer {
public:
	void renderScene(Scene& scene, size_t currentFrame, 
		bool passBaseMaterialProperties = false);
};

#endif // !SCENERENDERER_H
