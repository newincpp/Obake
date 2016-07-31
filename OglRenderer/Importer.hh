#include <stack>
#include "Mesh.hh"
#define GLEW_STATIC
#include <GL/glew.h>
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing fla
class Material {
};

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

class Importer {
    public:
	std::stack<Mesh> _meshBuffer;
	std::stack<Material> _materialBuffer;
	Importer(std::string file) {
	    load(file);
	}
	inline bool empty() {
	    return _meshBuffer.empty();
	}
	void genMesh(const aiScene* scene_);
	void load(std::string& file) {
	    // Create an instance of the Importer class
	    Assimp::Importer importer;
	    // And have it read the given file with some example postprocessing
	    // Usually - if speed is not the most important aspect for you - you'll 
	    // propably to request more postprocessing than we do in this example.
	    const aiScene* scene = importer.ReadFile(file, 
		    aiProcess_CalcTangentSpace       | 
		    aiProcess_Triangulate            |
		    aiProcess_JoinIdenticalVertices  |
		    aiProcess_SortByPType);

	    // If the import failed, report it
	    if( !scene) {
		std::cout << "fuu: " << importer.GetErrorString() << '\n';
	    }
	    genMesh(scene);
	}

};
