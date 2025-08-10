#ifndef __CSJOBJECT_H__
#define __CSJOBJECT_H__

#include <string>
#include <vector>

#include "CSJObjectIDAlloc.h"
#include "CSJSceneRuntime/function/framework/Component/CSJComponent.h"

namespace CSJEngine {

class CSJObject {
public:
    CSJObject();

    std::string getName() {
        return m_name;
    }

    void setName(std::string name) {
        m_name = name;
    }

    virtual ~CSJObject();

    virtual void tick(float delta_time);
private:
    CSJObjectID m_id {k_invalid_gobject_id};
    std::string m_name; 

    std::vector<CSJSpComponent> m_components;
};

using CSJSpObject = std::shared_ptr<CSJObject>;

} // namespace CSJEngine

#endif // __CSJOBJECT_H__