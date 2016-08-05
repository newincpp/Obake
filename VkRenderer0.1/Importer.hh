#pragma once

#include <stack>
#include "glm/glm.hpp"
#include "tiny_obj_loader.h"
#include "vulkan/vulkan.h"
#include "Mesh.hpp"


class Importer
{
public:
	std::stack<Mesh> _meshBuffer;
protected:
	// - - -
private:
	VkDevice			_device			= VK_NULL_HANDLE;
	VkPhysicalDevice	_physicalDevice = VK_NULL_HANDLE;

public:
	Importer(VkDevice & device_, VkPhysicalDevice &	physicalDevice_);

	void load(std::string file_);
	void uploadToGPU(VkCommandBuffer commandBuffer_);
	bool empty();

protected:
	void genMesh(std::vector<tinyobj::shape_t> & shapes_);
private:
	// - - -
};
