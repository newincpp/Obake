#pragma once

#include <Windows.h>

#include "ASystem.hh"

namespace System
{
	class WindowEvent : public Obake::ASystem
	{
	private:
		static WindowEvent *_sysInstance;

		HWND _hwnd;
		HINSTANCE _windowInstance;

	public:
		WindowEvent(const Obake::EventList& evRef_);

		static WindowEvent & getInstance()
		{
			Obake::EventList x;

			if (_sysInstance == nullptr)
				_sysInstance = new WindowEvent(x);
			return *_sysInstance;
		};

		void createWindow();
		void destroyWindow();

		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:

	};
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);