#pragma once

#include "PLATFORM.h"
#include "DEBUG.h"

#include "Debug.hpp"

namespace Platy
{
	class Device
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
		Device(VkInstance instance_, VkSurfaceKHR surface_);
		virtual ~Device();
	protected:
		// - - -
	private:
		VkInstance 			_instance = VK_NULL_HANDLE;
		VkSurfaceKHR 		_surface = VK_NULL_HANDLE;

		bool			_enableValidation = false;
		Platy::Debug	_debug;

		VkPhysicalDevice	_physicalDevice = VK_NULL_HANDLE;
		VkDevice			_device = VK_NULL_HANDLE;
		sQueueFamilyIndices _queueIndices;
		sQueueFamilyHandles _queueHandles;

	public:
		void createDevice();
		void destroyDevice();
		void pickPhysicalDevice();
		void createLogicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device_);
		void findQueueFamilies(VkPhysicalDevice device_, VkQueueFlags flags_);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device_);
	};
}