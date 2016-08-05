#include <ctime>
#include <stack>
#include "OglCore.hh"

void getGlError(const char* file_, unsigned long line_) {
    GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
	std::cout << "in file: \033[0;1m" << file_ << "\033[0m line: \033[1m" << line_ << "\033[0m: "; 
	if (e == GL_INVALID_ENUM) 				{ std::cout << "\033[31mGL_INVALID_ENUM\033[0m: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_INVALID_VALUE) 			{ std::cout << "\033[31mGL_INVAGL_INVALID_VALUE\033[0m: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_INVALID_OPERATION) 			{ std::cout << "\033[31mGL_INVAGL_INVALID_OPERATION\033[0m: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_INVALID_FRAMEBUFFER_OPERATION) 	{ std::cout << "\033[31mGL_INVAGL_INVALID_FRAMEBUFFER_OPERATION\033[0m: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n"; }
	else if (e == GL_OUT_OF_MEMORY) 			{ std::cout << "\033[31mGL_INVAGL_OUT_OF_MEMORY\033[0m: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n"; }
	else if (e == GL_STACK_UNDERFLOW) 			{ std::cout << "\033[31mGL_INVAGL_STACK_UNDERFLOW\033[0m: An attempt has been made to perform an operation that would cause an internal stack to underflow.\n"; }
	else if (e == GL_STACK_OVERFLOW) 			{ std::cout << "\033[31mGL_INVAGL_STACK_OVERFLOW\033[0m: An attempt has been made to perform an operation that would cause an internal stack to overflow.\n"; }
    }
}

void OglCore::import(std::string file_) {
    Importer iscene(file_);
    checkGlError;
    while (!iscene.empty()) {
	_scene.push_back(iscene._meshBuffer.top());
	iscene._meshBuffer.pop();
    }
    c = iscene._mainCamera;
}

void OglCore::renderScene() {
unsigned long i = 0;
    for (Mesh& m : _scene) {
	m.render();
	checkGlError;
    }
}

void OglCore::init() {
    _beginTime = std::chrono::high_resolution_clock::now();

    checkGlError glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    std::vector<GLfloat> vertices = {
    	//vertexPos		//normal		//uvCoord
        -1.0f,  1.0f,5.5f, 	0.0f,0.0f,1.0f, 	0.0f,0.0f, // Top-left
        1.0f,  1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	1.0f,0.0f, // Top-right
        1.0f, -1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	1.0f,1.0f, // Bottom-right
        -1.0f, -1.0f, 5.5f, 	0.0f,0.0f,1.0f, 	0.0f,1.0f  // Bottom-left
    };

    std::vector<GLuint> elements = {
        0, 1, 2,
        2, 3, 0
    };



    _sgBuffer.add("./fragGBuffer.glsl", GL_FRAGMENT_SHADER);
    _sgBuffer.add("./vertGBuffer.glsl", GL_VERTEX_SHADER);
    _sgBuffer.link({"vInfVertexPos_", "vInfVertexNormal_", "vInfUvCoord_"});

    _srender.add("./frag.glsl", GL_FRAGMENT_SHADER);
    _srender.add("./vert.glsl", GL_VERTEX_SHADER);
    _srender.link({"outColour"});

    _sPostProc.add("./postProcess.glsl",GL_FRAGMENT_SHADER);
    _sPostProc.add("./postProcessVert.glsl",GL_VERTEX_SHADER);
    _sPostProc.link({"outColour"});


    import("DemoCity.obj");
    Mesh m;
    m.uploadToGPU(vertices, elements);
    _scene.push_back(m);

    checkGlError _renderTarget.uploadToGPU(vertices, elements);

    _gBuffer.init();
    checkGlError;
    _gBuffer.addBuffer(); // Position
    _gBuffer.addBuffer(); // Normal
    _gBuffer.addBuffer(); // albedo
    _gBuffer.enable();
    checkGlError;
}

unsigned long OglCore::render() {
    std::chrono::time_point<std::chrono::high_resolution_clock> beginFrame = std::chrono::high_resolution_clock::now();
    GLfloat time = std::chrono::duration_cast<std::chrono::milliseconds>(beginFrame - _beginTime).count();
    uTime = time;
    uTime.upload();
    c.refresh();

    _sgBuffer.use();
    _gBuffer.enable();
    glClear(GL_DEPTH_BUFFER_BIT);
    renderScene();
    _gBuffer.disable();
    checkGlError;

    _srender.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderScene();

    //_sPostProc.use();
    //_gBuffer.bindGBuffer();
    //glClear(GL_DEPTH_BUFFER_BIT);
    //_renderTarget.render();
    //checkGlError;

    std::chrono::time_point<std::chrono::high_resolution_clock> endFrame = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(endFrame-beginFrame).count();
}
