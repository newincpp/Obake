#pragma once

#include "Core.hh"
#include "Plugin.hh"

namespace System
{
	class Renderer : public Obake::IPlugin
	{
		public:
		// - - -
		protected:
		bool _isExiting = false;
		HWND _winHandle = 0;
		HINSTANCE _winInstance = 0;
		private:
		// - - -
		
		public:
		Renderer();
		virtual ~Renderer();
		virtual void mainLoop();
		protected:
		// - - -
		private:
		virtual void initialize();
		void start();
		virtual void unload();
		
		void evSetExit(bool isExiting_);
		void evGetWinPtr(HWND winHandle_, HINSTANCE winInstance_);
	};
}
