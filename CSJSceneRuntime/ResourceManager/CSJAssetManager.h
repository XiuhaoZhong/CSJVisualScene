#ifndef __CSJASSETMANAGER_H__
#define __CSJASSETMANAGER_H__

#include <string>

#include "CSJSceneRuntime/function/framework/render/CSJRenderObjectData.h"

class CSJAssetManager {
public:
    CSJRenderMesh loadMeshFromFile(std::string& assetPath);
    CSJRenderMaterial loadMaterialFromFile(std::string& assetPath);

};

#endif // __CSJASSETMANAGER_H__