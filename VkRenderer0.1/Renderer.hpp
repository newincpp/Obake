#pragma once

#include "Core.hh"
#include "Plugin.hh"

namespace System
{
	class Renderer : public Obake::IPlugin
	{
	
	typedef struct
	{
	#ifdef _WIN32
		HWND winHandle = 0;
		HINSTANCE winInstance = 0;
	#elif __linux__
		xcb_connection_t* connection = nullptr;
		xcb_window_t window;
	#else
		// - - -
	#endif
	} sWinHandle;
	
	public:
		// - - -
	protected:
		bool 		_isExiting = false;
		sWinHandle	_winHandle;
	private:
		// - - -
		
	public:
		Renderer();
		virtual ~Renderer();
		virtual void mainLoop();
	protected:
		virtual void initialize();
		void start();
		virtual void unload();
	private:
		void evSetExit(bool isExiting_);
	#ifdef _WIN32
		void evGetWinPtr(HWND winHandle_, HINSTANCE winInstance_);
	#elif __linux__
		void evGetWinPtr(xcb_connection_t* connection_, xcb_window_t window_);
	#else
		// - - - 
	#endif
	};
}
