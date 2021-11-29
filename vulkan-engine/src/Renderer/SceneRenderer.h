#pragma once

#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include "Scene/Scene.h"
#include "Uniforms.h"
#include "EngineContext.h"
#include "VulkanContext.h"
#include "System.h"

class SceneRenderer {
public:
	void renderScene(size_t currentFrame, 
		bool passBaseMaterialProperties = false);

	void updateSceneUniforms(uint32_t currentFrame);
};

#endif // !SCENERENDERER_H
