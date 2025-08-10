#include "CSJObjectIDAlloc.h"

namespace CSJEngine {

std::atomic<CSJObjectID> CSJObjectIDAlloc::m_next_id {0};

CSJObjectID CSJObjectIDAlloc::alloc() {
    std::atomic<CSJObjectID> new_object_ret = m_next_id.load();
    m_next_id++;
    if (m_next_id >= k_invalid_gobject_id) {
        //LOG_FATAL("gobject id overflow");
    }

    return new_object_ret;
}

} // namespace CSJEngine