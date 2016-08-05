#pragma once

#include <stack>
#include "glm/glm.hpp"
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing fla
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

	void load(std::string file);
	void uploadToGPU(VkCommandBuffer commandBuffer_);
	bool empty();

protected:
	void genMesh(const aiScene* scene_);
private:
	// - - -
};
