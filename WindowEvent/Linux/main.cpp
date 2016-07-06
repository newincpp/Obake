#include <unistd.h>
#include "Window.hh"
#include <Core.hh>

int main() {
	Obake::Core c;
	sleep(1);
	System::Window win;
	c.registerSystem(&win);

	win.createWindow();
	// Run the message loop.
	while (win.alive()) {
		win.refresh();
	}
	return 0;
}
