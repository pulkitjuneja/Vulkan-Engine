#include "SceneRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"

void SceneRenderer::renderScene(FrameData& frame, bool passBaseMaterialProperties)
{
	auto device = EC::get()->vulkanContext->getDevice();
	auto swapChain = EC::get()->vulkanContext->getSwapChain();
	auto scene = EC::get()->scene;
	auto context = EC::get()->vulkanContext;
	//std::vector<FrameData>& frames = context->frames;

	vkWaitForFences(device.getLogicalDevice(), 1, &frame.inFlightfence, VK_TRUE, UINT64_MAX);
	vkResetFences(device.getLogicalDevice(), 1, &frame.inFlightfence);
	uint32_t nextImageIndex = swapChain.acquireNextImage(frame.imageAvailableSemaphore);

	vkResetCommandBuffer(frame.cmd.handle, 0);
	VkExtent2D swapChainExtents = swapChain.swapChainExtent;

	frame.cmd.beginRecording(0);
	frame.cmd.beginRenderPass(swapChain.screenRenderPass,
		swapChain.frameBuffers[nextImageIndex], swapChainExtents);

	std::vector<Entity>& entities = scene->getEntities();
	for (int i = 0 ; i < entities.size(); i++) 
	{
		// move this to scripts
		entities[i].transform.rotate(glm::vec3(0, 0.05f, 0));

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(frame.cmd.handle,
			0, 1, &entities[i].getMesh().getVBO(), &offset);

		vkCmdBindIndexBuffer(frame.cmd.handle,
			entities[i].getMesh().getEBO(), 0, VK_INDEX_TYPE_UINT16);

		entities[i].getMesh().subMeshes[0].material.pipeline->bind(frame.cmd);

		vkCmdBindDescriptorSets(frame.cmd.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, entities[i].getMesh().subMeshes[0].material.pipeline->pipelineLayout, 0, 1, &frame.frameDescriptorSet.handle, 0, nullptr);

		vkCmdBindDescriptorSets(frame.cmd.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, entities[i].getMesh().subMeshes[0].material.pipeline->pipelineLayout, 1, 1, &entities[i].getMesh().subMeshes[0].material.MaterialDescriptorSet[frame.frameIndex].handle, 0, nullptr);

		vkCmdDrawIndexed(frame.cmd.handle,
			static_cast<uint32_t>(entities[i].getMesh().getIndices().size()), 1, 0, 0, i);
	}

	frame.cmd.endRenderPass();
	frame.cmd.endRecording();

	std::vector<VkSemaphore> waitSemaphores = { frame.imageAvailableSemaphore};
	std::vector<VkSemaphore> signalSemaphores = { frame.renderFinishedSemaphore};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	std::vector<VkCommandBuffer> commandBuffers = { frame.cmd.handle };

	vk::CommandBuffer::submit(waitSemaphores, signalSemaphores, waitStages, commandBuffers, frame.inFlightfence);

	swapChain.presentImage(signalSemaphores, nextImageIndex);

}

void SceneRenderer::updateSceneUniforms(FrameData& frame)
{
	auto allocator = EC::get()->vulkanContext->allocator;
	auto scene = EC::get()->scene;

	PerFrameUniforms uniforms{};
	uniforms.projectionMatrix = scene->getMainCamera().getProjectionMatrix();
	uniforms.viewMatrix = scene->getMainCamera().getViewMatrix();
	uniforms.directionalLight = scene->getDirectionalLight();
	uniforms.cameraPosition = glm::vec4(scene->getMainCamera().transform.getPosition(), 1.0f);

	void* data;
	vmaMapMemory(allocator, frame.frameUniforms.allocation, &data);
	memcpy(data, &uniforms, sizeof(PerFrameUniforms));
	vmaUnmapMemory(allocator, frame.frameUniforms.allocation);

	void* objectData;
	vmaMapMemory(allocator, frame.objectUniforms.allocation, &objectData);
	PerObjectUniforms* objectSSBO = (PerObjectUniforms*)objectData;

	std::vector<Entity>& entities = scene->getEntities();
	for (int i = 0; i < entities.size(); i++)	
	{
		objectSSBO[i].modelMatrix = entities[i].transform.getTransformationMatrix();
	}
	vmaUnmapMemory(allocator, frame.objectUniforms.allocation);
}
