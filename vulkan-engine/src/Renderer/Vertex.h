#pragma once

#ifndef VERTEX_H
#define VERTEX_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include "VulkanHelpers.h"
#include "Material.h"
#include "vk_mem_alloc.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

struct SubMesh {
    Material material;
    uint32_t baseIndex;
    uint32_t indexCount;
    uint32_t baseVertex;
};

class Mesh {
protected:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    std::vector<SubMesh> subMeshes;
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;

public:
    Mesh(std::vector<Vertex>&& vertices, 
        std::vector<uint16_t>&& indices,
        std::vector<SubMesh> subMeshes);
    VkBuffer& getVBO() { return vertexBuffer.buffer; }
    VkBuffer& getEBO() { return indexBuffer.buffer; }
    std::vector<uint16_t>& getIndices() { return indices; }
    int getVertexCount() { return vertices.size(); }
    void release();
};

#endif // !VERTEX_H

 