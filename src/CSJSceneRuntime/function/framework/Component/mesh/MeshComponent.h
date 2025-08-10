#ifndef __MESHCOMPONENT_H__
#define __MESHCOMPONENT_H__

#include <vector>

#include "CSJSceneRuntime/function/framework/Component/CSJComponent.h"
#include "CSJSceneRuntime/resource/resource_type/components/mesh_type.h"
#include "CSJSceneRuntime/function/framework/render/RenderObjects.h"

namespace CSJEngine {

class MeshComponent : public CSJComponent {
public:
    MeshComponent();
    ~MeshComponent();

    void tick(float delta_time) override;
    void loadResource(std::weak_ptr<CSJObject> parent_object) override;

private:
    MeshComponentRes             m_meshRes;
    std::vector<GameObjPartDesc> m_rawMeshes;
};

} // namespace CSJEngine

#endif // __MESHCOMPONENT_H__