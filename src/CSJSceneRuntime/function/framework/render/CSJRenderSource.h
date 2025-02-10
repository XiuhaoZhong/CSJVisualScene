#ifndef __CSJRENDERSOURCE_H__
#define __CSJRENDERSOURCE_H__

#include "CSJRenderGuidManager.h"

#include <map>

class CSJRenderMesh {

};

class CSJRenderMaterial {

};

class CSJRenderSource {
public:

    // TODO: load and insert meshes;
    // TODO: load and insert materials;

private:
    std::map<size_t, CSJRenderMesh> m_csjMeshes;
    std::map<size_t, CSJRenderMaterial> m_csjMaterials;
};

#endif // __CSJRENDERSOURCE_H__