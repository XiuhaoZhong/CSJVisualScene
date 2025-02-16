#ifndef __CSJCOMPONENT_H__
#define __CSJCOMPONENT_H__

#include <memory>

#include "glm/glm.hpp"

class CSJObject;
class CSJComponent {
public:
    virtual ~CSJComponent();
    virtual void tick(float delta_time);    

protected:
    std::weak_ptr<CSJObject> m_parent_object;
    glm::mat4 m_transform;
};

using CSJSpComponent = std::shared_ptr<CSJComponent>;
#endif // __CSJCOMPONENT_H__