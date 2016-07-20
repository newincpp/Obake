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
	std::cout << "[BIND] Set Renderer Exit State" << std::endl;
	_core->eventsManager.bindEvent("Set Renderer Exit State", this, &Renderer::evSetExit);
	std::cout << "[BIND] Receive Window Info" << std::endl;
	_core->eventsManager.bindEvent("Receive Window Info", this, &Renderer::evGetWinPtr);
	
	OBAKE_ADD(start);
}

void
Renderer::start()
{
	std::cout << "[EXECUTE] Create Window" << std::endl;
	_core->eventsManager.executeEvent<void>("Create Window");
	std::cout << "[EXECUTE] Get Window Handle" << std::endl;
	_core->eventsManager.executeEvent<void>("Get Window Handle");
}

void
Renderer::unload()
{
	std::cout << "[EXECUTE] Destroy Window" << std::endl;
	_core->eventsManager.executeEvent<void>("Destroy Window");
}

// # # # # # # # # # # # # # # //

void
Renderer::evSetExit(bool isExiting_)
{
	_isExiting = isExiting_;
}

void
Renderer::evGetWinPtr(HWND winHandle_, HINSTANCE winInstance_)
{
	_winHandle = winHandle_;
	_winInstance = winInstance_;
}

// # # # # # # # # # # # # # # //

void
Renderer::mainLoop()
{
	_core->eventsManager.executeEvent<void>("Window Event");
}
