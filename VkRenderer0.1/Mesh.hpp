#pragma once

#include <vector>
#include <array>
#include <type_traits>

#include "Tools.hpp"

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

class Mesh
{
public:
protected:
	// - - -
private:
	VkDevice			_device			= VK_NULL_HANDLE;
	VkPhysicalDevice	_physicalDevice	= VK_NULL_HANDLE;

	VkBuffer		_vertexBuffer		= VK_NULL_HANDLE;
	VkDeviceMemory	_vertexBufferMemory = VK_NULL_HANDLE;


	VkDeviceSize _vertexSize;

public:
	Mesh(VkDevice & device_, VkPhysicalDevice &	physicalDevice_);

	void loadVertexBuffer(const std::vector<float> & verticesList_);
	void uploadToGPU(VkCommandBuffer commandBuffer_);
	uint32_t	findMemoryType(uint32_t typeFilter_, VkMemoryPropertyFlags properties_);

	void cleanup();
	void render();
protected:
	// - - -
private:
	// - - -
};
