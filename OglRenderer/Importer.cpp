#include <iostream>
#include "Importer.hh"


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
