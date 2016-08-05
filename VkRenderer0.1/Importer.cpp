#include <iostream>
#include "Importer.hh"

Importer::Importer(VkDevice & device_, VkPhysicalDevice &	physicalDevice_)
	: _device(device_), _physicalDevice(physicalDevice_)
{

}


void
Importer::load(std::string file_)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(shapes, materials, err, file_.c_str()))
		std::cerr << err << std::endl;

	genMesh(shapes);
}

void
Importer::genMesh(std::vector<tinyobj::shape_t> & shapes_)
{
	std::cout << "# of shapes    : " << shapes_.size() << std::endl;

	for (size_t i = 0; i < shapes_.size(); i++)
	{
		printf("shape[%ld].vertices: %ld\n", i, shapes_[i].mesh.positions.size());
		assert((shapes_[i].mesh.positions.size() % 3) == 0);
		for (size_t v = 0; v < shapes_[i].mesh.positions.size() / 3; v++)
		{
			printf("  v[%ld] = (%f, %f, %f)\n", v,
				shapes_[i].mesh.positions[3 * v + 0],
				shapes_[i].mesh.positions[3 * v + 1],
				shapes_[i].mesh.positions[3 * v + 2]);
		}
	}

//	for (std::remove_reference<decltype(scene_)>::type::value_type mesh : scene_)
//	{
//		std::vector<float> vertexBuffer;
//		aiMesh* mesh = scene_->mMeshes[m];
//		vertexBuffer.reserve(mesh->mNumVertices * 8);
//
//
//		for (int i = 0; i < mesh->mNumVertices; ++i)
//		{
//			vertexBuffer.push_back(mesh->mVertices[i].x);
//			vertexBuffer.push_back(mesh->mVertices[i].y);
//			vertexBuffer.push_back(mesh->mVertices[i].z);
//			vertexBuffer.push_back(mesh->mNormals[i].x);
//			vertexBuffer.push_back(mesh->mNormals[i].y);
//			vertexBuffer.push_back(mesh->mNormals[i].z);
//
//
//			if (mesh->HasTextureCoords(0))
//			{
//				vertexBuffer.push_back(mesh->mTextureCoords[0][i].x);
//				vertexBuffer.push_back(mesh->mTextureCoords[0][i].y);
//			}
//			else
//			{
//				vertexBuffer.push_back(0.0f);
//				vertexBuffer.push_back(0.0f);
//			}
//		}
//
//		_meshBuffer.emplace(_device, _physicalDevice);
//		_meshBuffer.top().loadVertexBuffer(vertexBuffer);
//	}
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
