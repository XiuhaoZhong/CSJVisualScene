#ifndef __CSJRENDERSOURCE_H__
#define __CSJRENDERSOURCE_H__

#include "CSJRenderGuidManager.h"

#include <map>

class CSJRenderMesh {

};

class CSJRenderMaterial {

};

/**
 * This class is responsible for managing render resource, including upload
 * resources into render core, loading material and so on.
 */
class CSJRenderResource {
public:
    CSJRenderResource();
    ~CSJRenderResource();
    // TODO: load and insert meshes;
    // TODO: load and insert materials;

private:
    std::map<size_t, CSJRenderMesh> m_csjMeshes;
    std::map<size_t, CSJRenderMaterial> m_csjMaterials;
};

#endif // __CSJRENDERSOURCE_H__