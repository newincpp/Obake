#include "MACRO.h"

#include "Renderer.hpp"

using namespace System;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

// # # # # # # # # # # # # # # //

void
Renderer::initialize()
{
	_core->eventsManager.bindEvent("Set Renderer Exit State", this, &Renderer::evSetExit);
	_core->eventsManager.bindEvent("Receive Window Info", this, &Renderer::evGetWinPtr);
	
	OBAKE_ADD(start);
}

void
Renderer::start()
{
	_core->eventsManager.executeEvent<void>("Create Window");
	_core->eventsManager.executeEvent<void>("Get Window Handle");
}

void
Renderer::unload()
{
	_core->eventsManager.executeEvent<void>("Destroy Window");
}

// # # # # # # # # # # # # # # //

void
Renderer::evSetExit(bool isExiting_)
{
	PRINT("## [RENDERER] [" << __FILE__ << "] EXIT STATE == " << isExiting_);	
	_isExiting = isExiting_;
}

#ifdef _WIN32
void
Renderer::evGetWinPtr(HWND winHandle_, HINSTANCE winInstance_)
{
	_winHandle.winHandle = winHandle_;
	_winHandle.winInstance = winInstance_;
}
#elif __linux__
void
Renderer::evGetWinPtr(xcb_connection_t* connection, xcb_window_t window)
{
	_winHandle.connection = connection_;
	_winHandle.window = window_;
}
#else
	// - - - 
#endif

// # # # # # # # # # # # # # # //

void
Renderer::mainLoop()
{
	_core->eventsManager.executeEvent<void>("Window Event");
}
