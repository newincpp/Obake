#pragma once
#include "PLATFORM.h"
#include "DEBUG.h"

#include "Renderer.hpp"
#include "VDeleter.hpp"

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
		bool _enableValidation;
		VDeleter<VkInstance> _instance{vkDestroyInstance};
	
	public:
		// - - -
	protected:
		virtual void initialize();
		virtual void unload();

		virtual void mainLoop();
	private:
		void		initVulkan();
		VkResult	createInstance(bool enableValidation_);
	};	
}

