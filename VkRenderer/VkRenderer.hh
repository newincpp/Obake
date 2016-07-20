#pragma once

#include "Core.hh"
#include "Plugin.hh"

#include "MACRO.h"
#include "BUILD_OPTIONS.h"
#include "PLATFORM.h"

// Sascha's code
#include "vulkanDebug.h"
#include "vulkanTools.h"
#include "vulkanSwapchain.h"
#include "vulkanTextureLoader.h"
#include "vulkanTextOverlay.h"

namespace System
{
	class VkRenderer : public Obake::IPlugin
	{
	private:
		// Set to true when example is created with enabled validation layers
		bool _enableValidation = false;
		// Set to true when the debug marker extension is detected
		bool _enableDebugMarkers = false;
	protected:
		// Vulkan instance, stores all per-application states
		VkInstance _instance;
		// Physical device (GPU) that Vulkan will ise
		VkPhysicalDevice _physicalDevice;
		// Stores physical device properties (for e.g. checking device limits)
		VkPhysicalDeviceProperties _deviceProperties;
		// Stores phyiscal device features (for e.g. checking if a feature is available)
		VkPhysicalDeviceFeatures _deviceFeatures;
		// Stores all available memory (type) properties for the physical device
		VkPhysicalDeviceMemoryProperties _deviceMemoryProperties;
		// Logical device, application's view of the physical device (GPU)
		VkDevice _device;
		// Handle to the device graphics queue that command buffers are submitted to
		VkQueue _queue;
		// The index to the queue that supports graphic operations
		uint32_t _graphicsQueueIndex;
		// Color buffer format
//		VkFormat _colorformat = VK_FORMAT_B8G8R8A8_UNORM;
		// Depth buffer format
		// Depth format is selected during Vulkan initialization
		VkFormat _depthFormat;
		// Command buffer pool
		VkCommandPool _cmdPool;
		// Command buffer used for setup
		VkCommandBuffer _setupCmdBuffer = VK_NULL_HANDLE;
		// Command buffer for submitting a post present image barrier
		VkCommandBuffer _postPresentCmdBuffer = VK_NULL_HANDLE;
		// Command buffer for submitting a pre present image barrier
		VkCommandBuffer _prePresentCmdBuffer = VK_NULL_HANDLE;
		// Pipeline stage flags for the submit info structure
		VkPipelineStageFlags _submitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		// Contains command buffers and semaphores to be presented to the queue
		VkSubmitInfo _submitInfo;
		// Command buffers used for rendering
		std::vector<VkCommandBuffer> _drawCmdBuffers;
		// Global render pass for frame buffer writes
		VkRenderPass _renderPass;
		// List of available frame buffers (same as number of swap chain images)
		std::vector<VkFramebuffer> _frameBuffers;
		// Active frame buffer index
//		uint32_t _currentBuffer = 0;
		// Descriptor set pool
		VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
		// List of shader modules created (stored for cleanup)
		std::vector<VkShaderModule> _shaderModules;
		// Pipeline cache object
		VkPipelineCache _pipelineCache;
		// Wraps the swap chain to present images (framebuffers) to the windowing system
		VulkanSwapChain _swapChain;

		// Synchronization semaphores
		struct {
			// Swap chain image presentation
			VkSemaphore presentComplete;
			// Command buffer submission and execution
			VkSemaphore renderComplete;
			// Text overlay submission and execution
			VkSemaphore textOverlayComplete;
		} _semaphores;

		// Simple texture loader
		vkTools::VulkanTextureLoader *_textureLoader = nullptr;
		// Returns the base asset path (for shaders, models, textures) depending on the os
		//const std::string getAssetPath();
	public:
//		bool _prepared = false;
//		uint32_t _width = 1280;
//		uint32_t _height = 720;
//
//		VkClearColorValue _defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };
//
//		float _zoom = 0;
//
//		// Defines a frame rate independent timer value clamped from -1.0...1.0
//		// For use in animations, rotations, etc.
//		float _timer = 0.0f;
//		// Multiplier for speeding up (or slowing down) the global timer
//		float _timerSpeed = 0.25f;
//
//		bool _paused = false;
//
//		bool _enableTextOverlay = false;
//		VulkanTextOverlay *_textOverlay;
//
//		// Use to adjust mouse rotation speed
//		float _rotationSpeed = 1.0f;
//		// Use to adjust mouse zoom speed
//		float _zoomSpeed = 1.0f;
//
//		glm::vec3 _rotation = glm::vec3();
//		glm::vec3 _cameraPos = glm::vec3();
//		glm::vec2 _mousePos;
//
//		std::string _title = "Vulkan Example";
//		std::string _name = "vulkanExample";
//
		struct
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		} _depthStencil;

		HWND _hwnd;
		HINSTANCE _hInstance;
		// # - # - # - # - #

	private:
		void initialize();
		void registerCore(Obake::Core*);
		void unload();
		void vulkanEvent();
		void eInitVulkan();
		void sendWinHandleEvent(HWND winHandle_, HINSTANCE winInstance_);
		void createSurface();

		void initVulkan(bool enableValidation_);
//		void initSwapchain();

		VkResult	createInstance(bool enableValidation_);
		VkResult	createDevice(VkDeviceQueueCreateInfo requestedQueues, bool enableValidation);
		void		createCommandPool();
		void		createSetupCommandBuffer();
		void		createCommandBuffers();

		void setupConsole(std::string title);
		void setupSwapChain();
		void buildPresentCommandBuffers();
	protected:
	public:
		VkRenderer(bool enableValidation_);
		~VkRenderer();

		void dieDieDie();

		virtual void prepare();

		void initSwapchain();

		// TEMPORARY SCHTUFF
		uint32_t width = 1280;
		uint32_t height = 720;

		HWND setupWindow(HINSTANCE hinstance, WNDPROC wndproc);
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	};
}