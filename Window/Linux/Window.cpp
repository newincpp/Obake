#include <cassert>
#include <iostream>

#include "Window.hh"
#include <Core.hh>

System::Window::Window() : _connection(nullptr), _screen(nullptr) {
	OBAKE_ADD(createWindow);
	OBAKE_LOOP {
		OBAKE_ADD(refresh);
	}
}

void System::Window::createWindow() {
	if (_core) {
		_core->eventsManager.bindEvent("Window Event", this, &Window::windowEvent);
		_core->eventsManager.bindEvent("Create Window", this, &Window::createWindow);
		_core->eventsManager.bindEvent("Get Windows Handle", this, &Window::sendWindowHandle);
	}
	const xcb_setup_t* setup;
	xcb_screen_iterator_t iter;
	int scr;

	_connection = xcb_connect(NULL, &scr);
	assert(_connection != NULL);

	setup = xcb_get_setup(_connection);
	iter = xcb_setup_roots_iterator(setup);
	while (scr-- > 0)
		xcb_screen_next(&iter);
	_screen = iter.data;
	_setupWindow();
}

void System::Window::destroyWindow() {
	xcb_destroy_window(_connection, _window);
	xcb_disconnect(_connection);
	_connection = nullptr;
}

void System::Window::sendWindowHandle() {
}

void System::Window::windowEvent() {
}


void System::Window::_setupWindow() {
	uint32_t value_list[2];
	uint16_t width = 1920;
	uint16_t height = 1024;

	_window = xcb_generate_id(_connection);

	value_list[0] = _screen->black_pixel;
	value_list[1] = XCB_EVENT_MASK_KEY_PRESS |
			XCB_EVENT_MASK_EXPOSURE |
			XCB_EVENT_MASK_STRUCTURE_NOTIFY |
			XCB_EVENT_MASK_POINTER_MOTION |
			XCB_EVENT_MASK_BUTTON_PRESS |
			XCB_EVENT_MASK_BUTTON_RELEASE;

	xcb_create_window(_connection,
			XCB_COPY_FROM_PARENT,
			_window, _screen->root,
			0, 0, width, height, 0, // TODO: put dynamic width and height
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			_screen->root_visual,
			XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, value_list);

	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(_connection, 1, 12, "WM_PROTOCOLS");
	xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(_connection, cookie, 0);

	xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(_connection, 0, 16, "WM_DELETE_WINDOW");
	_atom_wm_delete_window = xcb_intern_atom_reply(_connection, cookie2, 0);

	xcb_change_property(_connection, XCB_PROP_MODE_REPLACE,
			_window, (*reply).atom, 4, 32, 1,
			&(*_atom_wm_delete_window).atom);

	std::string windowTitle("test");
	xcb_change_property(_connection, XCB_PROP_MODE_REPLACE,
			_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
			windowTitle.size(), windowTitle.c_str());

	free(reply);

	xcb_map_window(_connection, _window);
	xcb_flush(_connection);
}

void System::Window::refresh() {
	_lastEvent = xcb_wait_for_event(_connection);

	if (_lastEvent->response_type == XCB_EXPOSE) {
		std::cout << "flush" << std::endl;
		xcb_flush(_connection);
	} else if (_lastEvent->response_type == XCB_BUTTON_PRESS) {
		std::cout << "mouse" << std::endl;
		destroyWindow();	
	} else if (_lastEvent->response_type == XCB_KEY_PRESS) { // "press" seems ignored while "released" seems to be called while pressed...
		std::cout << "keyboard" << std::endl;
		destroyWindow();	
	}

	std::cout << "refresh" << std::endl;
	if (_connection == nullptr) {
		shutdown();
	}
}
