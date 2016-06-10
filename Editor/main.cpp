#include "Core.hh"
#include "PluginsManager.hh"
//#include "TestEventSystem1.hh"
//#include "TestEventSystem2.hh"
//#include "VulkanRenderer.hh"

int main()
{
	Obake::Core c;
	//System::TestEventSystem1 system1;
	//System::TestEventSystem2 system2;
	//System::VulkanRenderer vulkan;

	//system1.registerCore(&c);
	//system2.registerCore(&c);
	//vulkan.registerCore(&c);
	//system1.initialize();
	//system2.initialize();
	//vulkan.initialize();

	//c.registerSystem(&system1);
	//c.registerSystem(&system2);

	//Obake::SharedLibrary testLib;

	//if (!testLib.open(".\\Plugins\\TestPlugin.dll"))
	//	std::cout << "Failed to open dll" << std::endl;
	//else
	//{
	//	Obake::PluginInfos* infos;

	//	testLib.sym("exports", reinterpret_cast<void**>(&infos));
	//	std::cout << "Plugin Info: "
	//		<< "\n\tAPI Version: " << infos->apiVersion
	//		<< "\n\tFile Name: " << infos->fileName
	//		<< "\n\tClass Name: " << infos->className
	//		<< "\n\tPlugin Name: " << infos->pluginName
	//		<< "\n\tPlugin Version: " << infos->pluginVersion
	//		<< std::endl;
	//}

	Obake::PluginsManager pluginsManager;

	pluginsManager.loadAllAvailablePlugins();
	pluginsManager.displayPluginsInfos();
	const std::vector<Obake::AvailablePlugin*>& availablePlugins = pluginsManager.getAllAvailablePlugins();

	for (Obake::AvailablePlugin* plugin : availablePlugins)
	{
		if (plugin->isLoaded())
		{
			c.registerSystem(plugin->getPlugin());
			plugin->getPlugin()->sayHello();
		}
	}


	// Vulkan tests
	/*
	{
	auto device = vkRender._device;
	auto queue = vkRender._queue;

	// SYNCHRONISATION VAR
	// Waits for the GPU to be ready on the CPU side
	VkFence fence;
	{
	VkFenceCreateInfo fenceCreateInfo =
	{
	// (MANDATORY) sType is the type of the structure
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	fenceCreateInfo.pNext = nullptr,
	// A flag that defines the initial state and behavior of the fence. Either signaled or unsignaled.
	fenceCreateInfo.flags = 0
	};
	vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
	}

	// Tells the GPU when other process in the GPU are completed
	VkSemaphore semaphore;
	{
	VkSemaphoreCreateInfo semaphoreCreateInfo =
	{
	// (MANDATORY) sType is the type of the structure
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	semaphoreCreateInfo.pNext = nullptr,
	// Flag reserved for future use
	semaphoreCreateInfo.flags = 0
	};

	vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
	}
	// - - -

	// COMMAND BUFFERS
	VkCommandPool commandPool;
	{
	VkCommandPoolCreateInfo poolCreateInfo =
	{
	// (MANDATORY) sType is the type of the structure
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	poolCreateInfo.pNext = nullptr,
	// A bitmask indicating usage behavior for the pool and command buffers allocated from it
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	// (NOTE) All command buffers created from this command pool must be submitted on queues from the same queue family.
	poolCreateInfo.queueFamilyIndex = vkRender._graphicsFamilyIndex
	};
	vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool);
	}

	VkCommandBuffer commandBuffer[2];
	{
	VkCommandBufferAllocateInfo commandBufferInfo =
	{
	// (MANDATORY) sType is the type of the structure
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	commandBufferInfo.pNext = nullptr,
	// The command pool that will be used to allocate the command buffer(s)
	commandBufferInfo.commandPool = commandPool,
	// Determines whether the command buffers are primary or secondary command buffers. Secondary command buffers are contained in primary command buffers.
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	// The number of command buffers you want to allocate
	commandBufferInfo.commandBufferCount = 2
	};

	vkAllocateCommandBuffers(device, &commandBufferInfo, commandBuffer);
	}
	// - - -

	// RECORDING IN COMMAND BUFFER(S)
	{
	{
	VkCommandBufferBeginInfo beginInfo =
	{
	// (MANDATORY) sType is the type of the structure
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	beginInfo.pNext = nullptr,
	// A bitmask indicating usage behavior for the command buffer.
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
	// A pointer to a VkCommandBufferInheritanceInfo structure, which is used if commandBuffer is a secondary command buffer.
	beginInfo.pInheritanceInfo = nullptr
	};
	vkBeginCommandBuffer(commandBuffer[0], &beginInfo);

	vkCmdPipelineBarrier(
	commandBuffer[0],
	VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	0,
	0, nullptr,
	0, nullptr,
	0, nullptr);

	// Filling the command buffer
	{
	VkViewport viewport =
	{
	viewport.maxDepth = 1.0f,
	viewport.minDepth = 0.0f,
	viewport.width = 512,
	viewport.height = 512,
	viewport.x = 0,
	viewport.y = 0
	};
	vkCmdSetViewport(commandBuffer[0], 0, 1, &viewport);
	}

	vkEndCommandBuffer(commandBuffer[0]);
	}

	{
	VkCommandBufferBeginInfo beginInfo =
	{
	// (MANDATORY) sType is the type of the structure
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	beginInfo.pNext = nullptr,
	// A bitmask indicating usage behavior for the command buffer.
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
	// A pointer to a VkCommandBufferInheritanceInfo structure, which is used if commandBuffer is a secondary command buffer.
	beginInfo.pInheritanceInfo = nullptr
	};
	vkBeginCommandBuffer(commandBuffer[1], &beginInfo);

	VkViewport viewport{};
	viewport.maxDepth = 1.0f;
	viewport.minDepth = 0.0f;
	viewport.width = 512;
	viewport.height = 512;
	viewport.x = 0;
	viewport.y = 0;
	vkCmdSetViewport(commandBuffer[1], 0, 1, &viewport);

	vkEndCommandBuffer(commandBuffer[1]);
	}

	{
	VkSubmitInfo submitInfo =
	{
	// (MANDATORY) sType is the type of the structure
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	submitInfo.pNext = nullptr,
	// The number of semaphores upon which to wait before executing the command buffers for the batch.
	submitInfo.waitSemaphoreCount = 0,
	// A pointer to an array of semaphores upon which to wait before executing the command buffers in the batch
	submitInfo.pWaitSemaphores = nullptr,
	// A pointer to an array of pipeline stages at which each corresponding semaphore wait will occur.
	submitInfo.pWaitDstStageMask = nullptr,
	// Contains the number of command buffers to execute in the batch
	submitInfo.commandBufferCount = 1,
	// A pointer to an array of command buffers to execute in the batch.
	// The command buffers submitted in a batch begin execution in the order they appear in pCommandBuffers, but may complete out of order
	submitInfo.pCommandBuffers = &commandBuffer[0],
	// The number of semaphores to be signaled once the commands specified in pCommandBuffers have completed execution.
	submitInfo.signalSemaphoreCount = 1,
	// A pointer to an array of semaphores which will be signaled when the command buffers for this batch have completed execution.
	submitInfo.pSignalSemaphores = &semaphore
	};

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	}
	{
	VkPipelineStageFlags flags[]{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
	VkSubmitInfo submitInfo =
	{
	// (MANDATORY) sType is the type of the structure
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
	submitInfo.pNext = nullptr,
	// The number of semaphores upon which to wait before executing the command buffers for the batch.
	submitInfo.waitSemaphoreCount = 1,
	// A pointer to an array of semaphores upon which to wait before executing the command buffers in the batch
	submitInfo.pWaitSemaphores = &semaphore,
	// A pointer to an array of pipeline stages at which each corresponding semaphore wait will occur.
	submitInfo.pWaitDstStageMask = flags,
	// Contains the number of command buffers to execute in the batch
	submitInfo.commandBufferCount = 1,
	// A pointer to an array of command buffers to execute in the batch.
	// The command buffers submitted in a batch begin execution in the order they appear in pCommandBuffers, but may complete out of order
	submitInfo.pCommandBuffers = &commandBuffer[1],
	// The number of semaphores to be signaled once the commands specified in pCommandBuffers have completed execution.
	submitInfo.signalSemaphoreCount = 1,
	// A pointer to an array of semaphores which will be signaled when the command buffers for this batch have completed execution.
	submitInfo.pSignalSemaphores = &semaphore
	};

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	}
	}

	// Synchronisation
	{
	//			vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkQueueWaitIdle(queue);
	}

	// Freeing up things that have been allocated
	{
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyFence(device, fence, nullptr);
	vkDestroySemaphore(device, semaphore, nullptr);
	}
	}
	*/

	//int runRet = c.run();
	system("Pause");
	//return runRet;
	return 0;
}
