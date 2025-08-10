#ifndef __CSJCOMPONENT_H__
#define __CSJCOMPONENT_H__

#include <memory>

#include "glm/glm.hpp"

namespace CSJEngine {

class CSJObject;
class CSJComponent {
public:
    CSJComponent() = default;
    virtual ~CSJComponent();
    virtual void tick(float delta_time);    

    virtual void loadResource(std::weak_ptr<CSJObject> parent_object) {
        m_pParentObject = parent_object;
    }

protected:
    std::weak_ptr<CSJObject> m_pParentObject;
    bool m_isDirty;
    bool m_isScaleDirty;
};

using CSJSpComponent = std::shared_ptr<CSJComponent>;

} // namespace CSJEngine 
#endif // __CSJCOMPONENT_H__