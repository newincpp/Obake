#include <stack>
#include "Mesh.hh"
#include "Camera.hh"
#include "glew.h"
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing fla

class Material {
};

class Importer {
    public:
	std::stack<Mesh> _meshBuffer;
	std::stack<Material> _materialBuffer;
	//Camera _mainCamera;
	Importer(std::string file);
	inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
	inline bool empty() {
	    return _meshBuffer.empty();
	}
	void genMesh(const aiScene* scene_);
	void load(std::string& file);
};
