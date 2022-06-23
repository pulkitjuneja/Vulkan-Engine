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
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoords);

        return attributeDescriptions;
}

//Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices, std::vector<SubMesh> subMeshes)
//{
//    this->vertices = vertices;
//    this->indices = indices;
//    this->subMeshes = subMeshes;
//
//    VmaAllocator alloc = EngineContext::get()->vulkanContext->allocator;
//    auto& gc = EC::get()->vulkanContext;
//
//    vk::Buffer stagingBuffer;
//
//    // create staging buffer for vertexBuffer
//    VkBufferCreateInfo stagingBuferInfo = vk::getBufferCreateinfo(vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
//    VmaAllocationCreateInfo vmaallocInfo = {};
//    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
//    if (vmaCreateBuffer(alloc, &stagingBuferInfo, &vmaallocInfo, &stagingBuffer.handle, &stagingBuffer.allocation, nullptr) != VK_SUCCESS) {
//        Logger::logError("failed to create staging buffer");
//    }
//    void* data;
//    vmaMapMemory(alloc, stagingBuffer.allocation, &data);
//    memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
//    vmaUnmapMemory(alloc, stagingBuffer.allocation);
//
//    // copy vertex buffer data to GPU only buffer
//    VkBufferCreateInfo vertexBufferInfo = vk::getBufferCreateinfo(vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
//    vmaallocInfo = {};
//    vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//    if (vmaCreateBuffer(alloc, &vertexBufferInfo, &vmaallocInfo, &this->vertexBuffer.handle, &this->vertexBuffer.allocation, nullptr) != VK_SUCCESS) {
//        Logger::logError("failed to create vertex buffer for mesh");
//    }
//    gc->immediateSubmit([=](VkCommandBuffer cmd) {
//        VkBufferCopy copy;
//        copy.dstOffset = 0;
//        copy.srcOffset = 0;
//        copy.size = vertices.size() * sizeof(Vertex);
//        vkCmdCopyBuffer(cmd, stagingBuffer.handle, vertexBuffer.handle, 1, &copy);
//        });
//    vmaDestroyBuffer(alloc, stagingBuffer.handle, stagingBuffer.allocation);
//
//    //create staging buffer for indices
//    stagingBuferInfo = vk::getBufferCreateinfo(indices.size() * sizeof(uint16_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
//    vmaallocInfo = {};
//    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
//    if (vmaCreateBuffer(alloc, &stagingBuferInfo, &vmaallocInfo, &stagingBuffer.handle, &stagingBuffer.allocation, nullptr) != VK_SUCCESS) {
//        Logger::logError("failed to create staging buffer");
//    }
//
//    vmaMapMemory(alloc, stagingBuffer.allocation, &data);
//    memcpy(data, indices.data(), indices.size() * sizeof(uint16_t));
//    vmaUnmapMemory(alloc, stagingBuffer.allocation);
//
//    //copy index data to gpu only buffer
//    VkBufferCreateInfo indexBufferInfo = vk::getBufferCreateinfo(indices.size() * sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
//    vmaallocInfo = {};
//    vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//    if (vmaCreateBuffer(alloc, &indexBufferInfo, &vmaallocInfo, &indexBuffer.handle, &indexBuffer.allocation, nullptr) != VK_SUCCESS) {
//        Logger::logError("failed to create index buffer");
//    }
//    gc->immediateSubmit([=](VkCommandBuffer cmd) {
//        VkBufferCopy copy;
//        copy.dstOffset = 0;
//        copy.srcOffset = 0;
//        copy.size = indices.size() * sizeof(uint16_t);
//        vkCmdCopyBuffer(cmd, stagingBuffer.handle, indexBuffer.handle, 1, &copy);
//        });
//    vmaDestroyBuffer(alloc, stagingBuffer.handle, stagingBuffer.allocation);
//}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices, std::vector<SubMesh> subMeshes)
{
    this->vertices = vertices;
    this->indices = indices;
    this->subMeshes = subMeshes;

    this->vertexBuffer.create(vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

    this->indexBuffer.create(indices.size() * sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

    // Create staging buffer for vertices
    vk::Buffer stagingBuffer;
    stagingBuffer.create(vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.copyData(vertices.data(), vertices.size() * sizeof(Vertex));
    stagingBuffer.copyToBuffer(this->vertexBuffer, 0, 0, vertices.size() * sizeof(Vertex));
    stagingBuffer.destroy();

    // create staging buffer for indices
    stagingBuffer.create(indices.size() * sizeof(uint16_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.copyData(indices.data(), indices.size() * sizeof(uint16_t));
    stagingBuffer.copyToBuffer(this->indexBuffer, 0, 0, indices.size() * sizeof(uint16_t));
    stagingBuffer.destroy();
}

void Mesh::release() {
    vertexBuffer.destroy();
    indexBuffer.destroy();
}
