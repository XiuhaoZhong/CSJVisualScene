#include "CSJRuntimeContext.h"

namespace CSJEngine {

CSJRuntimeContext *CSJEngine::CSJRuntimeContext::getRuntimeContextInstance() {
    static CSJRuntimeContext runtime_context_instance;
    return &runtime_context_instance;
}

bool CSJRuntimeContext::initialize() {

    m_render_system = std::make_shared<CSJRenderSystem>();

    return true;
}

}