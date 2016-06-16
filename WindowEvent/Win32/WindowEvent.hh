#pragma once

#include <Windows.h>

#include "Core.hh"
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

		static WindowEvent & getInstance()
		{
			if (_sysInstance == nullptr)
				_sysInstance = new WindowEvent();
			return *_sysInstance;
		};

		void registerCore(Obake::Core* core_);
		void initialize();
		void unload();

		void createSurface();
		void windowEventEvent();

		void createWindow();
		void destroyWindow();

		void messageLoop();
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:

	};
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);