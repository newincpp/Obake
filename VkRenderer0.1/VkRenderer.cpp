#include "VkRenderer.hpp"

using namespace System;

OBAKE_PLUGIN(VkRenderer, "Vulkan Renderer", "0.1.0")

VkRenderer::VkRenderer()
: Renderer()
{
}

VkRenderer::~VkRenderer()
{
}

void
VkRenderer::initialize()
{
	Renderer::initialize();
	OBAKE_ADD(mainLoop);
}

void
VkRenderer::unload()
{
	
}

void
VkRenderer::mainLoop()
{
	while (!_isExiting)
	{
		Renderer::mainLoop();
	}
	
}