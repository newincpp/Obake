#include "main.h"

VulkanExample::VulkanExample(bool enableDebug_)
	: VkRenderer(enableDebug_)
{}

VulkanExample::~VulkanExample()
{}

// # # # # # # #

/*
int
main()
{
	VulkanExample vkRenderer(true);

	vkRenderer.prepare();

	system("Pause");
	return 1;
}
*/


VulkanExample *vulkanExample;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (vulkanExample != NULL)
	{
		vulkanExample->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int main()
{

	vulkanExample = new VulkanExample(true);
	// This isn't staying
	vulkanExample->setupWindow(GetModuleHandle(NULL), WndProc);
	// - - 
	vulkanExample->initSwapchain();
	vulkanExample->prepare();
	//	vulkanExample->renderLoop();

	system("Pause");
	//	vulkanExample->dieDieDie();
	//	delete(vulkanExample);
	return 0;
}
