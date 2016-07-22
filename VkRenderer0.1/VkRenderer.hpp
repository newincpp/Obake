#pragma once
#include "PLATFORM.h"
#include "DEBUG.h"

#include "Renderer.hpp"
#include "VkDebug.hpp"


namespace System
{
	class VkRenderer : public System::Renderer
	{
	public:
		VkRenderer();
		virtual ~VkRenderer();	
	protected:
		// - - -
	private:
		bool _enableValidation = false;
		
		VkInstance 			_instance 		= VK_NULL_HANDLE;
		VkDebug 			_debug;
		VkPhysicalDevice	_physicalDevice = VK_NULL_HANDLE;
	
	public:
		// - - -
	protected:
		virtual void initialize();
		virtual void unload();

		virtual void mainLoop();
	private:
		void	initVulkan();
		void	createInstance();
		void	destroyInstance();
		void	createDevice();
		void	pickPhysicalDevice();
		bool	isDeviceSuitable(VkPhysicalDevice device_);
		bool	findQueueFamilies(VkPhysicalDevice device_, VkQueueFlags flags_);
	};	
}

