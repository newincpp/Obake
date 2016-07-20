#pragma once

#include "Renderer.hpp"

namespace System
{
	class VkRenderer : public System::Renderer
	{
		public:
		VkRenderer();
		virtual ~VkRenderer();
		
		virtual void mainLoop();

		private:
		virtual void initialize();
		virtual void unload();
	};	
}

