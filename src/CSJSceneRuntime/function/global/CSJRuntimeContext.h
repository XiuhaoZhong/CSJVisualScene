#ifndef __CSJGLOBALCONTEXT_H__
#define __CSJGLOBALCONTEXT_H__

#include <memory>

namespace CSJEngine {

class CSJRenderSystem;
class CSJWorldManager;

class CSJRuntimeContext {
public:
    static CSJRuntimeContext* getRuntimeContextInstance();
    CSJRuntimeContext() = default;
    ~CSJRuntimeContext() = default;

    bool initialize();

    std::shared_ptr<CSJRenderSystem> getRenderSystem() {
        return m_renderSystem;
    }

protected:


private:
    std::shared_ptr<CSJRenderSystem> m_renderSystem;
    std::shared_ptr<CSJWorldManager> m_worldManager;

};

}

#endif // __CSJGLOBALCONTEXT_H__