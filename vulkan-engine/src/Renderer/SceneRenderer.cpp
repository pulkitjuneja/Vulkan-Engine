#include "SceneRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"

void SceneRenderer::renderScene(Scene& scene, size_t currentFrame, bool passBaseMaterialProperties)
{
	std::vector<Entity> entities = scene.getEntities();

	auto device = EC::get()->vulkanContext->getDevice();
	auto swapChain = EC::get()->vulkanContext->getSwapChain();

	vkWaitForFences(device.getLogicalDevice(), 1, &swapChain.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device.getLogicalDevice(), 1, &swapChain.inFlightFences[currentFrame]);
	uint32_t nextImageIndex = swapChain.acquireNextImage(currentFrame);

	vkResetCommandBuffer(swapChain.screenCommandBUffers[currentFrame].commandBuffer, 0);
	VkExtent2D swapChainExtents = swapChain.swapChainExtent;

	swapChain.screenCommandBUffers[currentFrame].begin();

	for (Entity ent : entities) 
	{
		swapChain.screenCommandBUffers[currentFrame].beginRenderPass(ent.pipeline->getRenderPass(),
			swapChain.frameBuffers[nextImageIndex], swapChainExtents);
		swapChain.screenCommandBUffers[currentFrame].bindPipeline(ent.pipeline->getPipeline());
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(swapChain.screenCommandBUffers[currentFrame].commandBuffer,
			0, 1, &ent.getMesh().getVBO(), &offset);

		glm::vec3 camPos = { 0.f,0.f,-2.f };

		glm::mat4 view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//camera projection
		glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
		projection[1][1] *= -1;
		//model rotation
		glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(0.4f), glm::vec3(0, 1, 0));

		//calculate final mesh matrix
		glm::mat4 mesh_matrix = projection * view * model;

		PerObjectUniforms constants;
		constants.modelMatrix = mesh_matrix;

		//upload the matrix to the GPU via push constants
		vkCmdPushConstants(swapChain.screenCommandBUffers[currentFrame].commandBuffer,
			ent.pipeline->getPipelinelayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PerObjectUniforms), &constants);

		vkCmdDraw(swapChain.screenCommandBUffers[currentFrame].commandBuffer, ent.getMesh().getVertexCount(), 1, 0, 0);
		swapChain.screenCommandBUffers[currentFrame].endRenderPass();
		swapChain.screenCommandBUffers[currentFrame].end();
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	//
	VkSemaphore waitSemaphores[] = { swapChain.imageAvailableSemaphores[currentFrame] };
	VkSemaphore signalSemaphores[] = { swapChain.renderFinishedSemaphores[currentFrame] };

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &swapChain.screenCommandBUffers[currentFrame].commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	//
	if (vkQueueSubmit(device.queues.graphicsQueue, 1, &submitInfo, swapChain.inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapChain.vkSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &nextImageIndex;
	vkQueuePresentKHR(device.queues.presentQueue, &presentInfo);
}