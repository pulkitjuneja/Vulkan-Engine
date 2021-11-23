#include "SceneRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"

void SceneRenderer::renderScene(Scene& scene, size_t currentFrame, bool passBaseMaterialProperties)
{
	std::vector<Entity>& entities = scene.getEntities();

	auto device = EC::get()->vulkanContext->getDevice();
	auto swapChain = EC::get()->vulkanContext->getSwapChain();

	vkWaitForFences(device.getLogicalDevice(), 1, &swapChain.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device.getLogicalDevice(), 1, &swapChain.inFlightFences[currentFrame]);
	uint32_t nextImageIndex = swapChain.acquireNextImage(currentFrame);

	vkResetCommandBuffer(swapChain.screenCommandBUffers[currentFrame].commandBuffer, 0);
	VkExtent2D swapChainExtents = swapChain.swapChainExtent;

	swapChain.screenCommandBUffers[currentFrame].begin();

	for (int i = 0 ; i < entities.size(); i++) 
	{
		swapChain.screenCommandBUffers[currentFrame].beginRenderPass(swapChain.screenRenderPass,
			swapChain.frameBuffers[nextImageIndex], swapChainExtents);
		swapChain.screenCommandBUffers[currentFrame].bindPipeline(entities[i].pipeline->getPipeline());
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(swapChain.screenCommandBUffers[currentFrame].commandBuffer,
			0, 1, &entities[i].getMesh().getVBO(), &offset);

		vkCmdBindIndexBuffer(swapChain.screenCommandBUffers[currentFrame].commandBuffer, 
			entities[i].getMesh().getEBO(), 0, VK_INDEX_TYPE_UINT16);

		glm::vec3 camPos = { 0.f,0.f,-2.f };

		glm::vec3 eu0 = entities[i].transform.getEulerAngles();

		entities[i].transform.rotate(glm::vec3(0, 0.0005f, 0));

		glm::vec3 eu = entities[i].transform.getEulerAngles();

		glm::mat4 view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//camera projection
		glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
		projection[1][1] *= -1;

		glm::mat4 modelMatrix = entities[i].transform.getTransformationMatrix();

		//calculate final mesh matrix
		glm::mat4 mesh_matrix = projection * view * modelMatrix;

		PerObjectUniforms constants;
		constants.modelMatrix = mesh_matrix;

		//upload the matrix to the GPU via push constants
		vkCmdPushConstants(swapChain.screenCommandBUffers[currentFrame].commandBuffer,
			entities[i].pipeline->getPipelinelayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PerObjectUniforms), &constants);

		/*vkCmdDraw(swapChain.screenCommandBUffers[currentFrame].commandBuffer, ent.getMesh().getVertexCount(), 1, 0, 0);*/
		vkCmdDrawIndexed(swapChain.screenCommandBUffers[currentFrame].commandBuffer,
			static_cast<uint32_t>(entities[i].getMesh().getIndices().size()), 1, 0, 0, 0);
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