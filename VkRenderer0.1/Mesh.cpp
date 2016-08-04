#include "Mesh.hpp"

Mesh::Mesh(VkDevice & device_, VkPhysicalDevice & physicalDevice_)
	: _device(device_), _physicalDevice(physicalDevice_)
{
}

void
Mesh::loadVertexBuffer(const std::vector<System::VkRenderer::sVertex> vertices_)
{
	_vertexSize = vertices_.size();
	VkBufferCreateInfo bufferInfo =
	{
		// (MANDATORY) sType is the type of the structure
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		bufferInfo.pNext = nullptr,
		// A bitmask describing additional parameters of the buffer
		bufferInfo.flags = 0,
		// The size in bytes of the buffer to be created
		bufferInfo.size = sizeof(vertices_) * vertices_.size(),
		// A bitmask describing the allowed usages of the buffer
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		// The sharing mode of the buffer when it will be accessed by multiple queue families
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		// Is the number of entries in the pQueueFamilyIndices array
		bufferInfo.queueFamilyIndexCount = 0,
		// A list of queue families that will access this buffer
		bufferInfo.pQueueFamilyIndices = nullptr
	};

	vkCreateBuffer(_device, &bufferInfo, nullptr, &_vertexBuffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_device, _vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo =
	{
		// (MANDATORY) sType is the type of the structure
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		allocInfo.pNext = nullptr,
		// The size of the allocation in bytes
		allocInfo.allocationSize = memRequirements.size,
		// The memory type index, which selects the properties of the memory to be allocated, as well as the heap the memory will come from
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};

	vkAllocateMemory(_device, &allocInfo, nullptr, &_vertexBufferMemory);

	vkBindBufferMemory(_device, _vertexBuffer, _vertexBufferMemory, 0);

	void* data;
	vkMapMemory(_device, _vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices_.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(_device, _vertexBufferMemory);
}

uint32_t
Mesh::findMemoryType(uint32_t typeFilter_, VkMemoryPropertyFlags properties_)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter_ & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties_) == properties_)
		{
			return i;
		}
	}

	//	PRINT("## [VKRENDERER] [ERROR] [" << __FILE__ << "] FAILED TO FIND SUITABLE MEMORY TYPE");
}

void
Mesh::uploadToGPU(VkCommandBuffer commandBuffer_)
{
	VkBuffer vertexBuffers[] = { _vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer_, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer_, _vertexSize, 1, 0, 0);
}

void
Mesh::cleanup()
{
	vkFreeMemory(_device, _vertexBufferMemory, nullptr);
	vkDestroyBuffer(_device, _vertexBuffer, nullptr);
}

void
Mesh::render()
{
}
