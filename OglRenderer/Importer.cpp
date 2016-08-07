#include <iostream>
#include "Importer.hh"

Importer::Importer(std::string file) {
    load(file);
}

inline glm::mat4 Importer::aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;

    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}


void Importer::load(std::string& file) {
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
    aiMatrix4x4 m;
    //scene->mCameras[0]->GetCameraMatrix(m);
    if (scene->mNumCameras) {
	aiCamera* c = scene->mCameras[0];
	std::cout << "position: " << scene->mCameras[0]->mPosition[0] << ' ' << scene->mCameras[0]->mPosition[1] << ' ' << scene->mCameras[0]->mPosition[2] << '\n';
	//c->GetCameraMatrix(m);
	//_mainCamera.setMatrix(aiMatrix4x4ToGlm(m));

	_mainCamera.lookAt(glm::vec3(c->mLookAt[0], c->mLookAt[1], c->mLookAt[2]));
	_mainCamera.setPos(glm::vec3(c->mPosition[0] / 1.0f, c->mPosition[1] / 1.0f, c->mPosition[2] / 1.0f));
	//_mainCamera.fieldOfview(c->mHorizontalFOV);
	_mainCamera.fieldOfview(1.571f);
	_mainCamera.clipPlane(glm::vec2(c->mClipPlaneNear, c->mClipPlaneFar));
	_mainCamera.upVector(glm::vec3(c->mUp[0], c->mUp[1], c->mUp[2]));
    } else {
	std::cout << "no Camera Detected\n";
	//_mainCamera.lookAt(glm::vec3(136.0f,231.0f , 218.0f+ 1));
	_mainCamera.lookAt(glm::vec3(59.0f, 131.0f, 582.0f));
	_mainCamera.setPos(glm::vec3(136.0f, 231.0f, 218.0f));
	_mainCamera.fieldOfview(1.623f);
	_mainCamera.clipPlane(glm::vec2(0.1f, 10000.0f));
	_mainCamera.upVector(glm::vec3(0.0f,1.0f,0.0f));
    }
}

void Importer::genMesh(const aiScene* scene_) {
    for(unsigned int m = 0 ; m < scene_->mNumMeshes ; ++m) { 
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;
	aiMesh* mesh = scene_->mMeshes[m]; 
	vertexBuffer.reserve(mesh->mNumVertices * 8);
	indiceBuffer.reserve(mesh->mNumFaces * 3);


	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
	    vertexBuffer.push_back(mesh->mVertices[i].x);
	    vertexBuffer.push_back(mesh->mVertices[i].y);
	    vertexBuffer.push_back(mesh->mVertices[i].z);
	    vertexBuffer.push_back(mesh->mNormals[i].x);
	    vertexBuffer.push_back(mesh->mNormals[i].y);
	    vertexBuffer.push_back(mesh->mNormals[i].z);


	    if (mesh->HasTextureCoords(0)) {
		vertexBuffer.push_back(mesh->mTextureCoords[0][i].x);
		vertexBuffer.push_back(mesh->mTextureCoords[0][i].y);
	    } else {
		vertexBuffer.push_back(0.0f);
		vertexBuffer.push_back(0.0f);
	    }

	    //vertexBuffer.push_back(mesh->mBitangents[i].x);
	    //vertexBuffer.push_back(mesh->mBitangents[i].y);
	    //vertexBuffer.push_back(mesh->mBitangents[i].z);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[0]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[1]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[2]);
	}

	_meshBuffer.emplace();
	_meshBuffer.top().uploadToGPU(vertexBuffer, indiceBuffer);
	_meshBuffer.top()._name = mesh->mName.C_Str();

	aiNode* n = scene_->mRootNode->FindNode(mesh->mName);
	if (n) {
		_meshBuffer.top().uMeshTransform = aiMatrix4x4ToGlm(n->mTransformation);
	} else {
		std::cout << mesh->mName.C_Str() << " not found ! :'(\n";
	}
    } 
}
