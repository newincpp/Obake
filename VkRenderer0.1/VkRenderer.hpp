#pragma once
#include "PLATFORM.h"
#include "DEBUG.h"

#include "Renderer.hpp"
#include "Debug.hpp"


namespace System
{
	class VkRenderer : public System::Renderer
	{
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

	public:
		VkRenderer();
		virtual ~VkRenderer();	
	protected:
		// - - -
	private:
		bool			_enableValidation = false;
		Platy::Debug	_debug;

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
		// Window render pixel size
		VkExtent2D					_swapchainExtent;
		VkFormat					_swapchainImageFormat;


	
	public:
		virtual void mainLoop();
		virtual void initialize();
		virtual void unload();
	protected:
		// - - -
	private:
		void initVulkan();
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
		VkExtent2D					chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createImageViews();
		void destroyImageViews();
		void createGraphicsPipeline();
		void destroyGraphicsPipeline();
	};	
}

