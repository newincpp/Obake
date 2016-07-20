#include "VkRenderer.hpp"

using namespace System;

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