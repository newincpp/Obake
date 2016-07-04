#pragma once

#include "VkRenderer.hh"

class VulkanExample : public System::VkRenderer
{
public:
	VulkanExample(bool enableDebug_);
	~VulkanExample();
};