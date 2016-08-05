#include <iostream>
#include "Importer.hh"

Importer::Importer(VkDevice & device_, VkPhysicalDevice &	physicalDevice_)
	: _device(device_), _physicalDevice(physicalDevice_)
{

}


void
Importer::load(std::string file)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(file,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		std::cout << "fuu: " << importer.GetErrorString() << '\n';
	}
	genMesh(scene);
}

void
Importer::genMesh(const aiScene* scene_) {
	for (unsigned int m = 0; m < scene_->mNumMeshes; ++m)
	{
		std::vector<float> vertexBuffer;
		aiMesh* mesh = scene_->mMeshes[m];
		vertexBuffer.reserve(mesh->mNumVertices * 8);


		for (int i = 0; i < mesh->mNumVertices; ++i)
		{
			vertexBuffer.push_back(mesh->mVertices[i].x);
			vertexBuffer.push_back(mesh->mVertices[i].y);
			vertexBuffer.push_back(mesh->mVertices[i].z);
			vertexBuffer.push_back(mesh->mNormals[i].x);
			vertexBuffer.push_back(mesh->mNormals[i].y);
			vertexBuffer.push_back(mesh->mNormals[i].z);


			if (mesh->HasTextureCoords(0))
			{
				vertexBuffer.push_back(mesh->mTextureCoords[0][i].x);
				vertexBuffer.push_back(mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f);
			}
		}

		_meshBuffer.emplace(_device, _physicalDevice);
		_meshBuffer.top().loadVertexBuffer(vertexBuffer);
	}
}

void
Importer::uploadToGPU(VkCommandBuffer commandBuffer_)
{
	while (_meshBuffer.empty() == false)
	{
		_meshBuffer.top().uploadToGPU(commandBuffer_);
		_meshBuffer.top().cleanup();
		_meshBuffer.pop();
	}
}

bool
Importer::empty()
{
	return _meshBuffer.empty();
}
