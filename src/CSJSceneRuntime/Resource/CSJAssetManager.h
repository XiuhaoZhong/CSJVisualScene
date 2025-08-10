#ifndef __CSJASSETMANAGER_H__
#define __CSJASSETMANAGER_H__

#include <string>

#include "CSJSceneRuntime/function/framework/render/CSJRenderObjectData.h"

namespace CSJEngine {

class CSJAssetManager {
public:
    CSJRenderMesh loadMeshFromFile(std::string& assetPath);
    CSJRenderMaterial loadMaterialFromFile(std::string& assetPath);

};

} // namespace CSJEngine

#endif // __CSJASSETMANAGER_H__