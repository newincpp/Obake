#include <chrono>

#include "WindowEvent.hh"
#include "Core.hh"

using namespace System;

WindowEvent* WindowEvent::_sysInstance = nullptr;

WindowEvent::WindowEvent()
	: ASystem()
{}

void WindowEvent::createWindow()
{
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
	switch (uMsg)
	{
	case WM_CLOSE:
		std::cout << "Closing Window" << std::endl;
		break;
	case WM_QUIT:
		std::cout << "Quiting Window" << std::endl;
		break;
	default:
		return;
	}
}

///////////////////////////////////////////////////////////////

int main()
{
	Obake::Core c;
	System::WindowEvent winEvent = System::WindowEvent::getInstance();

	winEvent.createWindow();
	// Run the message loop.

	MSG msg = {};
	bool isExiting = false;
	while (isExiting == false)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_CLOSE)
			{
				std::cout << "QUITING" << std::endl;
				isExiting = true;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	system("Pause");
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowEvent::getInstance().handleMessages(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);;
}