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
    scene->mCameras[0]->GetCameraMatrix(m);
    std::cout << "position: " << scene->mCameras[0]->mPosition[0] << ' ' << scene->mCameras[0]->mPosition[1] << ' ' << scene->mCameras[0]->mPosition[2] << '\n';

    //_mainCamera.setMatrix(aiMatrix4x4ToGlm(m));
    //_mainCamera.fieldOfview(c->mHorizontalFOV);
    //_mainCamera.clipPlane(glm::vec2(c->mClipPlaneNear, c->mClipPlaneFar));
    //_mainCamera.upVector(glm::vec3(c->mUp[0], c->mUp[1], c->mUp[2]));
}

void Importer::genMesh(const aiScene* scene_) {
    for(unsigned int m = 0 ; m < scene_->mNumMeshes ; ++m) { 
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLuint> indiceBuffer;
	aiMesh* mesh = scene_->mMeshes[m]; 
	int iMeshFaces = mesh->mNumFaces; 
	vertexBuffer.reserve(mesh->mNumVertices * 8);
	indiceBuffer.reserve(mesh->mNumFaces * 3);


	for (int i = 0; i < mesh->mNumVertices; ++i) {
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
	for (int i = 0; i < mesh->mNumFaces; ++i) {
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[0]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[1]);
	    indiceBuffer.push_back(mesh->mFaces[i].mIndices[2]);
	}

	_meshBuffer.emplace();
	_meshBuffer.top().uploadToGPU(vertexBuffer, indiceBuffer);
	_meshBuffer.top()._name = mesh->mName.C_Str();
	//_meshBuffer.top()._transform = aiMatrix4x4ToGlm(scene_->mRootNode->mChildren[m]->mTransformation);
    } 
}
