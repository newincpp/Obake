#include <chrono>

#include "MACRO.h"

#include "Window.hpp"
#include "Core.hh"

using namespace System;

OBAKE_PLUGIN(Window, "Window Plugin", "0.1.0")

Window* Window::_sysInstance = nullptr;

Window::Window()
{}

Window::~Window()
{}

// Event System

void Window::initialize()
{
	if (_core != nullptr)
		getInstance()->_core = _core;
	// Create & bind an event
	_core->eventsManager.bindEvent("Window Event", this, &Window::evWindowEvent);
	_core->eventsManager.bindEvent("Create Window", this, &Window::evCreateWindow);
	_core->eventsManager.bindEvent("Destroy Window", this, &Window::evDestroyWindow);
	_core->eventsManager.bindEvent("Get Window Handle", this, &Window::evSendWindowHandle);
	// Call vulkan event
	//_core->eventsManager.executeEvent<void>("Vulkan Event");

// For now Renderer will handle calling messageLoop()
/*
	OBAKE_ADD(messageLoop);
	OBAKE_LOOP
	{
		OBAKE_ADD(messageLoop);
	}
*/
}


void Window::evSendWindowHandle()
{
	_core->eventsManager.executeEvent<void, HWND, HINSTANCE>("Receive Window Info", _hwnd, _windowInstance);
}

void Window::evWindowEvent()
{
	messageLoop();
}

void Window::registerCore(Obake::Core* core_)
{
	ASystem::registerCore(core_);
}

void Window::unload()
{

}

// - - !Event System


void Window::evCreateWindow()
{
	PRINT("## [WINDOW] [" << __FUNCTION__ << "] CREATE WINDOW");
	// Current Instance
	_windowInstance = GetModuleHandle(NULL);

	// Window param
	bool fullscreen = false;

	// Check command line arguments
	for (int32_t i = 0; i < __argc; i++)
	{
		if (__argv[i] == std::string("-fullscreen"))
		{
			fullscreen = true;
		}
	}

	// Register the window class.
	const char CLASS_NAME[] = "Sample Window Class";

	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _windowInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		std::cout << "RegisterClassEx() failed" << std::endl;
		std::cout << GetLastError() << std::endl;
		return;
	}

	// Create the window.
	_hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX, // Window style
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Size and position
		NULL,       // Parent window    
		NULL,       // Menu
		_windowInstance,  // Instance handle
		NULL        // Additional application data
		);

	if (_hwnd == NULL)
	{
		std::cout << "CreateWindowEx() failed" << std::endl;
		std::cout << "CreateWindowEx error : " << GetLastError() << std::endl;
		return;
	}

	ShowWindow(_hwnd, SW_SHOW);
}

void Window::evDestroyWindow()
{
	PRINT("## [WINDOW] [" << __FUNCTION__ << "] DESTROY WINDOW");
}

void Window::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Le code en commentaire est le code d'Adrien
//	wchar_t msg[32];

	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_QUIT:
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_SIZING :
	if (_core != nullptr)
		_core->eventsManager.executeEvent<void>("Window Update");
		break;
		//case WM_SYSKEYDOWN:
		//	swprintf_s(msg, L"WM_SYSKEYDOWN: 0x%x\n", wParam);
		//	OutputDebugString(msg);
		//	break;

		//case WM_SYSCHAR:
		//	swprintf_s(msg, L"WM_SYSCHAR: %c\n", (wchar_t)wParam);
		//	OutputDebugString(msg);
		//	break;

		//case WM_SYSKEYUP:
		//	swprintf_s(msg, L"WM_SYSKEYUP: 0x%x\n", wParam);
		//	OutputDebugString(msg);
		//	break;

		//case WM_KEYDOWN:
		//	swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", wParam);
		//	OutputDebugString(msg);
		//	break;

		//case WM_KEYUP:
		//	swprintf_s(msg, L"WM_KEYUP: 0x%x\n", wParam);
		//	OutputDebugString(msg);
		//	break;

		//case WM_CHAR:
		//	swprintf_s(msg, L"WM_CHAR: %c\n", (wchar_t)wParam);
		//	OutputDebugString(msg);
		//	break;
	default:
		return;
	}
}

void Window::messageLoop()
{
	//std::cout << "MESSAGE LOOP" << std::endl;
	bool isExiting = false;

	while (PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
	{
		if (_msg.message == WM_CLOSE || _msg.message == WM_QUIT)
		{
			isExiting = true;
			_core->eventsManager.executeEvent<void, bool>("Set Renderer Exit State", isExiting);
		}
		TranslateMessage(&_msg);
		DispatchMessage(&_msg);
	}
}

///////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window::getInstance()->handleMessages(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);;
}