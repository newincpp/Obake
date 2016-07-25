#pragma once
#include "PLATFORM.h"
#include "DEBUG.h"

#include "Renderer.hpp"
#include "VkDebug.hpp"


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

	public:
		VkRenderer();
		virtual ~VkRenderer();	
	protected:
		// - - -
	private:
		bool _enableValidation = false;
		
		VkInstance 			_instance 		= VK_NULL_HANDLE;
		VkDebug*			_debug			= VK_NULL_HANDLE;
		VkSurfaceKHR		_surface 		= VK_NULL_HANDLE;

		VkPhysicalDevice	_physicalDevice = VK_NULL_HANDLE;
		VkDevice			_device = VK_NULL_HANDLE;
		sQueueFamilyIndices _queueIndices;
		sQueueFamilyHandles _queueHandles;
	
	public:
		virtual void mainLoop();
		virtual void initialize();
		virtual void unload();
	protected:
		// - - -
	private:
		void				initVulkan();
		void				createInstance();
		void				destroyInstance();
		void				createSurface();
		void				destroySurface();
		void				createDevice();
		void				destroyDevice();
		void				pickPhysicalDevice();
		bool				isDeviceSuitable(VkPhysicalDevice device_, std::vector<VkDeviceQueueCreateInfo> * queueCreateInfos_);
		void				findQueueFamilies(VkPhysicalDevice device_, VkQueueFlags flags_);
	};	
}

