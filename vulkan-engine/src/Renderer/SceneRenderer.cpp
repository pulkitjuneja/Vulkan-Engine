#include "SceneRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"

void SceneRenderer::renderScene(size_t currentFrame, bool passBaseMaterialProperties)
{
	auto device = EC::get()->vulkanContext->getDevice();
	auto swapChain = EC::get()->vulkanContext->getSwapChain();
	auto scene = EC::get()->scene;
	std::vector<FrameData>& frames = EC::get()->vulkanContext->frames;

	vkWaitForFences(device.getLogicalDevice(), 1, &frames[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device.getLogicalDevice(), 1, &frames[currentFrame].inFlightFence);
	uint32_t nextImageIndex = swapChain.acquireNextImage(currentFrame);

	vkResetCommandBuffer(frames[currentFrame].FrameCommandBuffer.commandBuffer, 0);
	VkExtent2D swapChainExtents = swapChain.swapChainExtent;

	frames[currentFrame].FrameCommandBuffer.begin();
	frames[currentFrame].FrameCommandBuffer.beginRenderPass(swapChain.screenRenderPass,
		swapChain.frameBuffers[nextImageIndex], swapChainExtents);

	std::vector<Entity>& entities = scene->getEntities();
	for (int i = 0 ; i < entities.size(); i++) 
	{
		// move this to scripts
		entities[i].transform.rotate(glm::vec3(0, 0.0005f, 0));

		frames[currentFrame].FrameCommandBuffer.bindPipeline(entities[i].pipeline->pipeline);
		vkCmdBindDescriptorSets(frames[currentFrame].FrameCommandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			entities[i].pipeline->pipelineLayout, 0, 1, &frames[currentFrame].frameDescriptor, 0, nullptr);

		vkCmdBindDescriptorSets(frames[currentFrame].FrameCommandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			entities[i].pipeline->pipelineLayout, 1, 1, &frames[currentFrame].objectDescriptor, 0, nullptr);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(frames[currentFrame].FrameCommandBuffer.commandBuffer,
			0, 1, &entities[i].getMesh().getVBO(), &offset);

		vkCmdBindIndexBuffer(frames[currentFrame].FrameCommandBuffer.commandBuffer,
			entities[i].getMesh().getEBO(), 0, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(frames[currentFrame].FrameCommandBuffer.commandBuffer,
			static_cast<uint32_t>(entities[i].getMesh().getIndices().size()), 1, 0, 0, i);
	}

	frames[currentFrame].FrameCommandBuffer.endRenderPass();
	frames[currentFrame].FrameCommandBuffer.end();

	std::vector<VkSemaphore> waitSemaphores = { frames[currentFrame].imageAvailableSemaphore };
	std::vector<VkSemaphore> signalSemaphores = { frames[currentFrame].renderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	std::vector<VkCommandBuffer> commandBuffers = { frames[currentFrame].FrameCommandBuffer.commandBuffer };
	VkSubmitInfo submitInfo = vkInit::getSubmitInfo(waitSemaphores, signalSemaphores, waitStages, commandBuffers);

	if (vkQueueSubmit(device.queues.graphicsQueue, 1, &submitInfo, frames[currentFrame].inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	std::vector<VkSwapchainKHR> swapChains = { swapChain.vkSwapChain };
	VkPresentInfoKHR presentInfo = vkInit::getPresentInfo(signalSemaphores, swapChains, &nextImageIndex);

	vkQueuePresentKHR(device.queues.presentQueue, &presentInfo);
}

void SceneRenderer::updateSceneUniforms(uint32_t currentFrame)
{
	auto allocator = EC::get()->vulkanContext->allocator;
	auto scene = EC::get()->scene;
	std::vector<FrameData>& frames = EC::get()->vulkanContext->frames;

	PerFrameUniforms uniforms{};
	uniforms.projectionMatrix = scene->getMainCamera().getProjectionMatrix();
	uniforms.viewMatrix = scene->getMainCamera().getViewMatrix();
	uniforms.directionalLight = scene->getDirectionalLight();
	uniforms.cameraPosition = glm::vec4(scene->getMainCamera().transform.getPosition(), 1.0f);

	void* data;
	vmaMapMemory(allocator, frames[currentFrame].frameUniforms.allocation, &data);
	memcpy(data, &uniforms, sizeof(PerFrameUniforms));
	vmaUnmapMemory(allocator, frames[currentFrame].frameUniforms.allocation);

	void* objectData;
	vmaMapMemory(allocator, frames[currentFrame].objectBuffer.allocation, &objectData);
	PerObjectUniforms* objectSSBO = (PerObjectUniforms*)objectData;

	std::vector<Entity>& entities = scene->getEntities();
	for (int i = 0; i < entities.size(); i++)	
	{
		objectSSBO[i].modelMatrix = entities[i].transform.getTransformationMatrix();
	}
	vmaUnmapMemory(allocator, frames[currentFrame].objectBuffer.allocation);
}
