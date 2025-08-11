#include "CSJRuntimeContext.h"

#include "CSJSceneRuntime/function/render/render_system.h"
#include "CSJSceneRuntime/function/framework/WorldManager/CSJWorldManager.h"

namespace CSJEngine {

CSJRuntimeContext *CSJEngine::CSJRuntimeContext::getRuntimeContextInstance() {
    static CSJRuntimeContext runtime_context_instance;
    return &runtime_context_instance;
}

bool CSJRuntimeContext::initialize() {

    m_renderSystem = std::make_shared<CSJRenderSystem>();
    m_worldManager = std::make_shared<CSJWorldManager>();

    return true;
}

}