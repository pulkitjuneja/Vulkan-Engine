#pragma once

#ifndef VERTEX_H
#define VERTEX_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

#endif // !VERTEX_H
//
//class Mesh {
//protected:
//    std::vector<Vertex> vertices;
//    AllocatedBuffer vertexBuffer;
//};

 