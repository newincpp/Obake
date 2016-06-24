#include "VulkanRenderer.hh"

int main()
{
	System::VulkanRenderer vkRender;
	vkRender.initialize();
	auto device = vkRender._device;

	VkCommandPool commandPool;
	VkCommandPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.queueFamilyIndex = vkRender._graphicsFamilyIndex;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool);

	system("Pause");
	return 0;
}
