#include "SceneRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Renderer/Uniforms.h"

void SceneRenderer::renderScene(Scene& scene, size_t currentFrame, bool passBaseMaterialProperties)
{
	auto device = EC::get()->vulkanContext->getDevice();
	auto swapChain = EC::get()->vulkanContext->getSwapChain();
	auto allocator = EC::get()->vulkanContext->allocator;
	std::vector<FrameData>& frames = EC::get()->vulkanContext->frames;

	vkWaitForFences(device.getLogicalDevice(), 1, &frames[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device.getLogicalDevice(), 1, &frames[currentFrame].inFlightFence);
	uint32_t nextImageIndex = swapChain.acquireNextImage(currentFrame);

	vkResetCommandBuffer(frames[currentFrame].FrameCommandBuffer.commandBuffer, 0);
	VkExtent2D swapChainExtents = swapChain.swapChainExtent;

	frames[currentFrame].FrameCommandBuffer.begin();
	frames[currentFrame].FrameCommandBuffer.beginRenderPass(swapChain.screenRenderPass,
		swapChain.frameBuffers[nextImageIndex], swapChainExtents);

	PerFrameUniforms uniforms{};
	uniforms.projectionMatrix = scene.getMainCamera().getProjectionMatrix();
	uniforms.viewMatrix = scene.getMainCamera().getViewMatrix();

	// update uniform buffers
	void* data;
	vmaMapMemory(allocator, frames[currentFrame].frameUniforms.allocation, &data);
	memcpy(data, &uniforms, sizeof(PerFrameUniforms));
	vmaUnmapMemory(allocator, frames[currentFrame].frameUniforms.allocation);

	std::vector<Entity>& entities = scene.getEntities();
	for (int i = 0 ; i < entities.size(); i++) 
	{
		// move this to scripts
		entities[i].transform.rotate(glm::vec3(0, 0.0005f, 0));

		frames[currentFrame].FrameCommandBuffer.bindPipeline(entities[i].pipeline->pipeline);
		vkCmdBindDescriptorSets(frames[currentFrame].FrameCommandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			entities[i].pipeline->pipelineLayout, 0, 1, &frames[currentFrame].frameDescriptor, 0, nullptr);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(frames[currentFrame].FrameCommandBuffer.commandBuffer,
			0, 1, &entities[i].getMesh().getVBO(), &offset);

		vkCmdBindIndexBuffer(frames[currentFrame].FrameCommandBuffer.commandBuffer,
			entities[i].getMesh().getEBO(), 0, VK_INDEX_TYPE_UINT16);

		PerObjectUniforms constants;
		constants.modelMatrix = entities[i].transform.getTransformationMatrix();

		vkCmdPushConstants(frames[currentFrame].FrameCommandBuffer.commandBuffer,
			entities[i].pipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PerObjectUniforms), &constants);

		vkCmdDrawIndexed(frames[currentFrame].FrameCommandBuffer.commandBuffer,
			static_cast<uint32_t>(entities[i].getMesh().getIndices().size()), 1, 0, 0, 0);
		frames[currentFrame].FrameCommandBuffer.endRenderPass();
		frames[currentFrame].FrameCommandBuffer.end();
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	//
	VkSemaphore waitSemaphores[] = { frames[currentFrame].imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { frames[currentFrame].renderFinishedSemaphore };

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &frames[currentFrame].FrameCommandBuffer.commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	//
	if (vkQueueSubmit(device.queues.graphicsQueue, 1, &submitInfo, frames[currentFrame].inFlightFence) != VK_SUCCESS) {
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