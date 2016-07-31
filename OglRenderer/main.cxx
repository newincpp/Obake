#include "OglCore.hh"

#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

class MinimalWindow {
    public:
	GLFWwindow* _window;
	OglCore& _c;
	MinimalWindow(OglCore& c_) : _c(c_) {
	    glfwInit();
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	    _window = glfwCreateWindow(1920, 1024, "OpenGL", nullptr, nullptr); // Windowed
	    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
	    glfwMakeContextCurrent(_window);
	    glewExperimental = GL_TRUE;
	    glewInit();
	    std::cout << glGetString(GL_VENDOR) << '\n';
	    std::cout << glGetString(GL_VERSION) << '\n';
	    std::cout << glGetString(GL_RENDERER) << '\n';
	    std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	    _c.init();
	}
	void loop() {

	    while(!glfwWindowShouldClose(_window))
	    {
		std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
		long rendertime = _c.render();

		glfwSwapBuffers(_window);
		glfwPollEvents();
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		    glfwSetWindowShouldClose(_window, GL_TRUE);
		std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
		//std::cout << "GPU: " << rendertime << "ns\t\t" << 1000000000.0/double(rendertime) << "fps (theorical)\n"; 
		//std::cout << "CPU: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns\t\t" << 1000000000.0/double(std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()) << "fps\n\n"; 
	    }
	}
	~MinimalWindow() {
	    glfwTerminate();
	}
};

int main()
{
    OglCore c;
    MinimalWindow w(c);
    w.loop();
}
