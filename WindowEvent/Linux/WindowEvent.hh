#pragma once

#include <xcb/xcb.h>

#include <ASystem.hh>

namespace System
{
	class WindowEvent : public Obake::ASystem
	{
		private:
			xcb_connection_t* _connection;
			xcb_screen_t* _screen;
			xcb_window_t _window;
			xcb_intern_atom_reply_t* _atom_wm_delete_window;

			xcb_generic_event_t *_lastEvent;
			void _setupWindow();
		public:
			WindowEvent(const Obake::EventList& evRef_);

			void createWindow();
			void destroyWindow();
			void refresh();
			inline bool alive() { return (_connection != nullptr); }
	};
}
