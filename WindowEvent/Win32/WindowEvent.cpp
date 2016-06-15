#include <chrono>

#include "WindowEvent.hh"
#include "Core.hh"

using namespace System;

OBAKE_PLUGIN(WindowEvent, "WindowEvent Plugin", "0.1.0")

WindowEvent* WindowEvent::_sysInstance = nullptr;

WindowEvent::WindowEvent()
{
}

WindowEvent::~WindowEvent()
{}

// Event System

void WindowEvent::initialize()
{
	//	createWindow();

	// Create & bind an event
	_core->eventsManager.bindEvent("Window Event", this, &WindowEvent::windowEventEvent);
	// Call vulkan event
	//_core->eventsManager.executeEvent<void>("Vulkan Event");

	OBAKE_ADD(&WindowEvent::createWindow)
	OBAKE_ADD(&WindowEvent::sendWinHandle)
	OBAKE_ADD(&WindowEvent::windowEventEvent)
	OBAKE_LOOP
	{
		OBAKE_ADD(&WindowEvent::messageLoop)
	}
}


void WindowEvent::sendWinHandle()
{
	_core->eventsManager.executeEvent<void, HWND>("SendWinHandle", _hwnd);
}

void WindowEvent::windowEventEvent()
{
	//std::cout << "WINDOW EVENT" << std::endl;
}

void WindowEvent::registerCore(Obake::Core* core_)
{
	ASystem::registerCore(core_);
}

void WindowEvent::unload()
{

}

// - - !Event System


void WindowEvent::createWindow()
{
	std::cout << "CREATE WINDOW" << std::endl;
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

void WindowEvent::destroyWindow()
{

}

void WindowEvent::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Le code en commentaire est le code d'Adrien
//	wchar_t msg[32];

	switch (uMsg)
	{
	case WM_CLOSE:
		std::cout << "Closing Window" << std::endl;
		break;
	case WM_QUIT:
		std::cout << "Quiting Window" << std::endl;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
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

void WindowEvent::messageLoop()
{
	//std::cout << "MESSAGE LOOP" << std::endl;
	bool isExiting = false;

	while (PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
	{
		if (_msg.message == WM_CLOSE)
		{
			std::cout << "QUITING" << std::endl;
			isExiting = true;
		}
		TranslateMessage(&_msg);
		DispatchMessage(&_msg);
	}
	//return isExiting;
}

///////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowEvent::getInstance().handleMessages(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);;
}