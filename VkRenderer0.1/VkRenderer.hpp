#pragma once

#include "PLATFORM.h"
#include "DEBUG.h"

#include "Renderer.hpp"
#include "Debug.hpp"

#include "Importer.hh"

namespace System
{
	class VkRenderer : public Renderer
	{
	public:
	typedef struct
	{
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool isComplete()
		{
			return graphicsFamily >= 0 && presentFamily >= 0;
		}
	} sQueueFamilyIndices;

	typedef struct
	{
		VkQueue graphicsQueue;
		VkQueue presentQueue;
	} sQueueFamilyHandles;

	typedef struct
	{
		VkSurfaceCapabilitiesKHR		capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>	presentModes;
	} sSwapChainSupportDetails;

	typedef struct
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uvMap;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription =
			{
				// The binding number that this structure describes.
				bindingDescription.binding = 0,
				// The distance in bytes between two consecutive elements within the buffer
				bindingDescription.stride = sizeof(sVertex),
				// Specifies whether vertex attribute addressing is a function of the vertex index or of the instance index.
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(sVertex, pos);

			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(sVertex, normal);

			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(sVertex, uvMap);

			return attributeDescriptions;
		};
	}  sVertex;

	public:
		// - - -
	protected:
		// - - -
	private:
		bool			_enableValidation = false;
		Platy::Debug	_debug;
		bool			_debugMarker = false;

		Importer * _meshImporter;

		// Vulkan
		VkInstance 					_instance 		= VK_NULL_HANDLE;
		VkSurfaceKHR				_surface		= VK_NULL_HANDLE;
		VkPhysicalDevice			_physicalDevice = VK_NULL_HANDLE;
		VkDevice					_device			= VK_NULL_HANDLE;
		VkSwapchainKHR				_swapchain		= VK_NULL_HANDLE;

		// A struct that will contain the different indices of different queues
		sQueueFamilyIndices _queueIndices;
		// A struct that will contain the handle to these queues
		sQueueFamilyHandles _queueHandles;

		// Handles to all images in the swapchain
		std::vector<VkImage>		_swapchainImages;
		std::vector<VkImageView>	_swapchainImageViews;
		std::vector<VkFramebuffer>	_swapchainFramebuffers;
		// Window render pixel size
		VkExtent2D					_swapchainExtent;
		VkFormat					_swapchainImageFormat;

		VkShaderModule _vertShaderModule = VK_NULL_HANDLE;
		VkShaderModule _fragShaderModule = VK_NULL_HANDLE;

		VkRenderPass		_renderPass			= VK_NULL_HANDLE;
		VkPipelineLayout	_pipelineLayout		= VK_NULL_HANDLE;
		VkPipeline			_graphicsPipeline	= VK_NULL_HANDLE;

		VkCommandPool _commandPool = VK_NULL_HANDLE;

		VkBuffer		_vertexBuffer		= VK_NULL_HANDLE;
		VkDeviceMemory	_vertexBufferMemory	= VK_NULL_HANDLE;

		std::vector<VkCommandBuffer> _commandBuffers;

		VkSemaphore _imageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore _renderFinishedSemaphore = VK_NULL_HANDLE;
		// !Vulkan
	
	public:
		VkRenderer();
		virtual ~VkRenderer();

		virtual void mainLoop();
		virtual void initialize();
		virtual void unload();
	protected:
		void evUpdateSurface();
	private:

		void drawFrame();

		// Vulkan
		void initVulkan();
		void recreateSwapchain();

		void createInstance();
		void destroyInstance();

		void createSurface();
		void destroySurface();

		void createDevice();
		void destroyDevice();
		void pickPhysicalDevice();
		void createLogicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device_);
		void findQueueFamilies(VkPhysicalDevice device_, VkQueueFlags flags_);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device_);

		void						createSwapchain();
		void						destroySwapchain();
		sSwapChainSupportDetails	querySwapChainSupport(VkPhysicalDevice device_);
		VkSurfaceFormatKHR			chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats_);
		VkPresentModeKHR			chooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes);
		VkExtent2D					chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities_);

		void createImageViews();
		void destroyImageViews();

		void createRenderPass();
		void destroyRenderPass();

		void createGraphicsPipeline();
		void destroyGraphicsPipeline();
		void createShaderModule(const std::vector<char>& code_, VkShaderModule& shaderModule_);
		void destroyShaderModule();

		void createFramebuffers();
		void destroyFramebuffers();

		void createCommandPool();
		void destroyCommandPool();

		void		createVertexBuffer();
		void		destroyVertexBuffer();
		uint32_t	findMemoryType(uint32_t typeFilter_, VkMemoryPropertyFlags properties_);

			void createCommandBuffers();
		void destroyCommandBuffers();

		void createSemaphore();
		void destroySemaphore();

		// !Vulkan
	};	

}
