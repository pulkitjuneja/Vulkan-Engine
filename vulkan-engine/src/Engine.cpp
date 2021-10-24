#include "Engine.h"

void Engine::recordCommandBuffers()
{
	int count = graphicsContext->getSwapChain().frameBuffers.size();
	screenCommandBUffers.resize(count);

	VkCommandPool graphicsPool = graphicsContext->getGraphicsCommandPool();
	VkExtent2D swapChainExtents = graphicsContext->getSwapChain().swapChainExtent;

	for (int i = 0; i < count; i++) {
		screenCommandBUffers[i].initialize(graphicsPool);
		screenCommandBUffers[i].begin();
		screenCommandBUffers[i].beginRenderPass(pipeline.getRenderPass(),
			graphicsContext->getSwapChain().frameBuffers[i], swapChainExtents);
		screenCommandBUffers[i].bindPipeline(pipeline.getPipeline());
		screenCommandBUffers[i].draw(3);
		screenCommandBUffers[i].endRenderPass();
		screenCommandBUffers[i].end();
	}
}

void Engine::renderLoop()
{
	auto device = graphicsContext->getDevice();
	vkWaitForFences(device.getLogicalDevice(), 1, &graphicsContext->getSwapChain().inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device.getLogicalDevice(), 1, &graphicsContext->getSwapChain().inFlightFences[currentFrame]);
	uint32_t nextImageIndex = graphicsContext->getSwapChain().acquireNextImage(currentFrame);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {graphicsContext->getSwapChain().imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &screenCommandBUffers[nextImageIndex].commandBuffer;
	VkSemaphore signalSemaphores[] = {graphicsContext->getSwapChain().renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsContext->getDevice().queues.graphicsQueue, 1, &submitInfo, 
		graphicsContext->getSwapChain().inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { graphicsContext->getSwapChain().vkSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &nextImageIndex;
	vkQueuePresentKHR(device.queues.presentQueue, &presentInfo);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::start()
{
	window = std::make_unique<Window>(800, 600, "Vulkan Engine");
	EngineContext::get()->window = window.get();

	isEngineRunning = true;

	try {
		graphicsContext = std::make_unique<VulkanContext>();
		EngineContext::get()->vulkanContext = graphicsContext.get();
		graphicsContext->initialize();
		pipeline.createDefault();
		pipeline.createShaderStage("shaders/triangleVert.spv", "shaders/triangleFrag.spv");
		pipeline.build();
		graphicsContext->getSwapChain().createFrameBuffers(graphicsContext->getDevice(),
			pipeline.getRenderPass());
		recordCommandBuffers();

		// See if this can be reorderd
		graphicsContext->getSwapChain().createSemaphores();
	}
	catch (const std::exception& e) {
		Logger::logError(e.what());
		isEngineRunning = false;
	}

	while (isEngineRunning) {
		if (glfwWindowShouldClose(window->getNativeWindow())) {
			isEngineRunning = false;
		}
		window->display();
		renderLoop();
	}
	vkDeviceWaitIdle(graphicsContext->getDevice().logicalDevice);
}

Engine::~Engine()
{
	graphicsContext->getSwapChain().destroySwapFrameBuffers(graphicsContext->getDevice());
	pipeline.release();
	graphicsContext->release();
	window->shutdown();
}
