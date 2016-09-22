#pragma once

#include <iostream>
#include <vector>
#include "Camera.hh"
#include "AssetTypes.hh"

class Scene {
    private:
	std::vector<Asset3D> _assets;
    public:
	std::vector<Camera> _cameras;
	Shader _sgBuffer;
	unsigned int _activeCamera;
	Scene() : _activeCamera(1) {
	    _cameras.reserve(32);
	}
	void load() {
	    _sgBuffer.add("./fragGBuffer.glsl", GL_FRAGMENT_SHADER);
	    _sgBuffer.add("./vertGBuffer.glsl", GL_VERTEX_SHADER);
	    _sgBuffer.link({"gPosition", "gNormal", "gAlbedoSpec"});

	    _sgBuffer.referenceUniform(_cameras[0].uCamera, "uCamera");
	    //_sgBuffer.referenceUniform(m.uMeshTransform, "uMeshTransform");
	}
	inline void allocateAsset(unsigned long x) {
	    _assets.reserve(x);
	}
	Asset3D& emplaceAsset() {
	    _assets.emplace_back(&_sgBuffer);
	    _sgBuffer.referenceUniform(_assets.back()._mesh.uMeshTransform, "uMeshTransform");
	    return _assets.back();
	}
	void render() {
	    _sgBuffer.use();
	    for (unsigned int a = 0; a < _activeCamera; ++a) {
		_cameras[a].use();
		for (Asset3D& asset : _assets) {
		    asset.update();
		}
		_cameras[a].unUse();
	    }
	}
	void bindGBuffer(unsigned int camera_) {
	    _cameras[camera_].bindFramebuffer();
	}
};
