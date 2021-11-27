#include "Vertex.h"
#include "EngineContext.h"
#include "Renderer/VulkanContext.h"
#include "Logger.h"

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions()
{
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices)
{
    this->vertices = vertices;
    this->indices = indices;

    VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;

    VkBufferCreateInfo vertexBufferInfo = vkInit::getBufferCreateinfo(vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (vmaCreateBuffer(alloc, &vertexBufferInfo, &vmaallocInfo, &vertexBuffer.buffer, &vertexBuffer.allocation, nullptr) != VK_SUCCESS) {
        Logger::logError("failed to create vertex buffer for mesh");
    }

    VkBufferCreateInfo indexBufferInfo = vkInit::getBufferCreateinfo(indices.size() * sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    if (vmaCreateBuffer(alloc, &indexBufferInfo, &vmaallocInfo, &indexBuffer.buffer, &indexBuffer.allocation, nullptr) != VK_SUCCESS) {
        Logger::logError("failed to create index buffer for mesh");
    }

    void* data;
    vmaMapMemory(alloc, vertexBuffer.allocation, &data);
    memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(alloc, vertexBuffer.allocation);

    vmaMapMemory(alloc, indexBuffer.allocation, &data);
    memcpy(data, indices.data(), indices.size() * sizeof(uint16_t));
    vmaUnmapMemory(alloc, indexBuffer.allocation);
}

void Mesh::release() {
    VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;
    vmaDestroyBuffer(alloc, indexBuffer.buffer, indexBuffer.allocation);
    vmaDestroyBuffer(alloc, vertexBuffer.buffer, vertexBuffer.allocation);
}
