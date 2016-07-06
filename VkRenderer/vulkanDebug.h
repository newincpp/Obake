/*
* Texture loader for Vulkan
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once
#include "PLATFORM.h"

#include "vulkanTools.h"

class VkDebug
{
public:
	static VkDebug *s_instance;
	// Default validation layers
	int							_validationLayerCount;
	std::vector<const char*>	_validationLayerNames;

	// Debug Create Info
	VkDebugReportCallbackCreateInfoEXT _debugCallbackCreateInfo = {};

	PFN_vkCreateDebugReportCallbackEXT	CreateDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDebugReportMessageEXT			DebugBreakCallback = VK_NULL_HANDLE;

	//Variale given to createDebugReportCallback that will be filled with info
	VkDebugReportCallbackEXT _msgCallback = VK_NULL_HANDLE;

	// Fills the _debugCallbackCreateInfo struct
	void initDebugging(VkDebugReportFlagsEXT flags_);

	// Load debug function pointers and set debug callback
	// if callBack is NULL, default message callback will be used
	void setupDebugging(VkInstance instance_, VkDebugReportCallbackEXT callBack_);
	// Clear debug callback
	void freeDebugCallback(VkInstance instance);

	static VkDebug &instance()
	{
		if (!s_instance)
			s_instance = new VkDebug;
		return *s_instance;
	}

	VkDebug();
	~VkDebug();

	void dieDieDie();

	VkInstance _instance;

	// Setup and functions for the VK_EXT_debug_marker_extension
	// Extension spec can be found at https://github.com/KhronosGroup/Vulkan-Docs/blob/1.0-VK_EXT_debug_marker/doc/specs/vulkan/appendices/VK_EXT_debug_marker.txt
	// Note that the extension will only be present if run from an offline debugging application
	// The actual check for extension presence and enabling it on the device is done in the example base class
	// See VulkanExampleBase::createInstance and VulkanExampleBase::createDevice (base/vulkanexamplebase.cpp)
/*	namespace DebugMarker
	{
		// Set to true if function pointer for the debug marker are available
		extern bool active;

		// Get function pointers for the debug report extensions from the device
		void setup(VkDevice device);

		// Sets the debug name of an object
		// All Objects in Vulkan are represented by their 64-bit handles which are passed into this function
		// along with the object type
		void setObjectName(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char *name);

		// Set the tag for an object
		void setObjectTag(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag);

		// Start a new debug marker region
		void beginRegion(VkCommandBuffer cmdbuffer, const char* pMarkerName, glm::vec4 color);

		// Insert a new debug marker into the command buffer
		void insert(VkCommandBuffer cmdbuffer, std::string markerName, glm::vec4 color);

		// End the current debug marker region
		void endRegion(VkCommandBuffer cmdBuffer);

		// Object specific naming functions
		void setCommandBufferName(VkDevice device, VkCommandBuffer cmdBuffer, const char * name);
		void setQueueName(VkDevice device, VkQueue queue, const char * name);
		void setImageName(VkDevice device, VkImage image, const char * name);
		void setSamplerName(VkDevice device, VkSampler sampler, const char * name);
		void setBufferName(VkDevice device, VkBuffer buffer, const char * name);
		void setDeviceMemoryName(VkDevice device, VkDeviceMemory memory, const char * name);
		void setShaderModuleName(VkDevice device, VkShaderModule shaderModule, const char * name);
		void setPipelineName(VkDevice device, VkPipeline pipeline, const char * name);
		void setPipelineLayoutName(VkDevice device, VkPipelineLayout pipelineLayout, const char * name);
		void setRenderPassName(VkDevice device, VkRenderPass renderPass, const char * name);
		void setFramebufferName(VkDevice device, VkFramebuffer framebuffer, const char * name);
		void setDescriptorSetLayoutName(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const char * name);
		void setDescriptorSetName(VkDevice device, VkDescriptorSet descriptorSet, const char * name);
		void setSemaphoreName(VkDevice device, VkSemaphore semaphore, const char * name);
		void setFenceName(VkDevice device, VkFence fence, const char * name);
		void setEventName(VkDevice device, VkEvent _event, const char * name);
	};
*/
};
//namespace AvkDebug
//{
//	VkDebug instance;
//	int plop;
//}
//VkDebug instance;
//int plop;
