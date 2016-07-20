#pragma once

#include <Windows.h>
#include "Core.hh"
#include "Plugin.hh"

namespace System
{
	class Window : public Obake::IPlugin
	{

	private:
		static Window *_sysInstance;
		HWND _hwnd;
		HINSTANCE _windowInstance;

		MSG _msg;

	public:
		explicit Window();
		~Window();

		static Window * getInstance()
		{
			if (_sysInstance == nullptr)
				_sysInstance = new Window();
			return _sysInstance;
		};

		void registerCore(Obake::Core* core_);
		void initialize();
		void unload();

		void evSendWindowHandle();
		void evWindowEvent();

		void evCreateWindow();
		void evDestroyWindow();

		void messageLoop();
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:

	};
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);