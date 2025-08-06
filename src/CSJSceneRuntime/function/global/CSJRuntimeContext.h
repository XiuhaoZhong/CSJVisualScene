#ifndef __CSJGLOBALCONTEXT_H__
#define __CSJGLOBALCONTEXT_H__

#include <memory>

#include "CSJSceneRuntime/function/render/render_system.h"

namespace CSJEngine {

class CSJRuntimeContext {
public:
    static CSJRuntimeContext* getRuntimeContextInstance();
    CSJRuntimeContext() = default;
    ~CSJRuntimeContext() = default;

    bool initialize();

    std::shared_ptr<CSJRenderSystem> getRenderSystem() {
        return m_render_system;
    }

protected:


private:
    std::shared_ptr<CSJRenderSystem> m_render_system;

};

}

#endif // __CSJGLOBALCONTEXT_H__