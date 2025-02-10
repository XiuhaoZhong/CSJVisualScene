#ifndef __CSJOBJECT_H__
#define __CSJOBJECT_H__

#include <string>

#include "CSJSceneRuntime/function/framework/Object/CSJObjectIDAlloc.h"

class CSJObject {
public:
    CSJObject();
    virtual ~CSJObject();

    virtual void tick();
private:
    CSJObjectID m_id {k_invalid_gobject_id};
    std::string m_name; 
};

using CSJSpObject = std::shared_ptr<CSJObject>;

#endif // __CSJOBJECT_H__