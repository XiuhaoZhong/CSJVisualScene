#ifndef __MESHCOMPONENT_H__
#define __MESHCOMPONENT_H__

#include "CSJComponent.h"

#include <vector>

#include "CSJSceneRuntime/function/framework/render/RenderObjects.h"

class MeshComponent : public CSJComponent {
public:
    MeshComponent();
    ~MeshComponent();

    void tick(float delta_time) override;
    void loadResource(std::weak_ptr<CSJObject> parent_object) override;

private:
    std::vector<GameObjPartDesc> m_raw_meshes;
};

#endif // __MESHCOMPONENT_H__