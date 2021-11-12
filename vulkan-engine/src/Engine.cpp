#include "Engine.h"

void Engine::initCommandBuffer()
{
	screenCommandBUffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandPool graphicsPool = graphicsContext->getGraphicsCommandPool();
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		screenCommandBUffers[i].initialize(graphicsPool);
	}
	mainCommandBuffer.initialize(graphicsPool);
}

void Engine::initMeshes()
{
	std::vector<Vertex> triangleVertices;
	triangleVertices.resize(3);

	//vertex positions
	triangleVertices[0].position = { 0.5f, 0.5f, 0.0f };
	triangleVertices[1].position = { -0.5f, 0.5f, 0.0f };
	triangleVertices[2].position = { 0.f,-0.5f, 0.0f };

	//vertex colors, all green
	triangleVertices[0].color = { 1.f, 0.0f, 0.0f }; //pure green
	triangleVertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
	triangleVertices[2].color = { 0.f, 0.0f, 1.0f }; //pure green

	Scene.push_back(Mesh(std::move(triangleVertices)));
}

void Engine::renderLoop()
{
	auto device = graphicsContext->getDevice();
	vkWaitForFences(device.getLogicalDevice(), 1, &graphicsContext->getSwapChain().inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device.getLogicalDevice(), 1, &graphicsContext->getSwapChain().inFlightFences[currentFrame]);
	uint32_t nextImageIndex = graphicsContext->getSwapChain().acquireNextImage(currentFrame);

	vkResetCommandBuffer(screenCommandBUffers[currentFrame].commandBuffer, 0);

	VkExtent2D swapChainExtents = graphicsContext->getSwapChain().swapChainExtent;

	screenCommandBUffers[currentFrame].begin();
	screenCommandBUffers[currentFrame].beginRenderPass(pipeline.getRenderPass(),
		graphicsContext->getSwapChain().frameBuffers[nextImageIndex], swapChainExtents);
	screenCommandBUffers[currentFrame].bindPipeline(pipeline.getPipeline());

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(screenCommandBUffers[currentFrame].commandBuffer,
		0, 1, &Scene[0].getVBO(), &offset);

	vkCmdDraw(screenCommandBUffers[currentFrame].commandBuffer, Scene[0].getVertexCount(), 1, 0, 0);
	screenCommandBUffers[currentFrame].endRenderPass();
	screenCommandBUffers[currentFrame].end();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
//
	VkSemaphore waitSemaphores[] = {graphicsContext->getSwapChain().imageAvailableSemaphores[currentFrame]};
	VkSemaphore signalSemaphores[] = { graphicsContext->getSwapChain().renderFinishedSemaphores[currentFrame] };

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &screenCommandBUffers[currentFrame].commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
//
	if (vkQueueSubmit(graphicsContext->getDevice().queues.graphicsQueue, 1, &submitInfo, 
		graphicsContext->getSwapChain().inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}
//
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
	window = std::make_unique<Window>(1366, 768, "Vulkan Engine");
	EngineContext::get()->window = window.get();

	isEngineRunning = true;

	try {
		graphicsContext = std::make_unique<VulkanContext>();
		EngineContext::get()->vulkanContext = graphicsContext.get();
		graphicsContext->initialize();
		pipeline.build("shaders/triangleVert.spv", "shaders/triangleFrag.spv");
		graphicsContext->getSwapChain().createFrameBuffers(graphicsContext->getDevice(),
			pipeline.getRenderPass());
		initCommandBuffer();

		// See if this can be reorderd
		graphicsContext->getSwapChain().createSemaphores();
	}
	catch (const std::exception& e) {
		Logger::logError(e.what());
		isEngineRunning = false;
	}


	initMeshes();

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
	for (Mesh mesh : Scene) {
		mesh.release();
	}
	graphicsContext->getSwapChain().destroySwapFrameBuffers(graphicsContext->getDevice());
	pipeline.release();
	graphicsContext->release();
	window->shutdown();
}
