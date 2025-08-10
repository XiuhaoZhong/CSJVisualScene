#include "CSJObject.h"

#include "CSJSceneRuntime/function/framework/Component/CSJComponent.h"

namespace CSJEngine {
    
void CSJObject::tick(float delta_time) {
    for (auto component : m_components) {
        if (!component) {
            continue;
        }
        
        component->tick(delta_time);
    }
}

} // namespace CSJEngine