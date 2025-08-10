#ifndef __MESH_TYPE_H__
#define __MESH_TYPE_H__

#include <string>
#include <vector>

namespace CSJEngine {

class SubMeshRes {
public:
    SubMeshRes() = default;
    ~SubMeshRes() = default;

    std::string m_objFile;
    std::string m_material;
    // TODO: m_transform definition.
};

class MeshComponentRes {
public:
    MeshComponentRes() = default;
    ~MeshComponentRes() = default;

    std::vector<SubMeshRes> m_subMeshes;
};

} // namespace CSJEngine
#endif // __MESH_TYPE_H__