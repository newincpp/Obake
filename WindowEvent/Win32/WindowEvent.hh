#pragma once

#include <Windows.h>

#include "Plugin.hh"

namespace System
{
	class WindowEvent : public Obake::IPlugin
	{
	private:
		static WindowEvent *_sysInstance;
		HWND _hwnd;
		HINSTANCE _windowInstance;

		MSG _msg;

	public:
		explicit WindowEvent();
		~WindowEvent();

		void initialize();

		static WindowEvent & getInstance()
		{
			if (_sysInstance == nullptr)
				_sysInstance = new WindowEvent();
			return *_sysInstance;
		};

		void sayHello() { };

		void createWindow();
		void destroyWindow();

		bool messageLoop();
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:

	};
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);