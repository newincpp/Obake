#include "VkRenderer.hpp"

using namespace System;

OBAKE_PLUGIN(VkRenderer, "Vulkan Renderer", "0.1.0")

VkRenderer::VkRenderer()
: Renderer()
{
	OBAKE_LOOP
	{
		OBAKE_ADD(mainLoop);
	}
}

VkRenderer::~VkRenderer()
{
	
}

void
VkRenderer::initialize()
{
	
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