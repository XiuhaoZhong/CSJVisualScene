#ifndef __CSJOBJECTIDALLOC_H__
#define __CSJOBJECTIDALLOC_H__

#include <atomic>
#include <limits>

namespace CSJEngine {

using CSJObjectID = std::size_t;

constexpr CSJObjectID k_invalid_gobject_id = std::numeric_limits<std::size_t>::max();

class CSJObjectIDAlloc {
public:
    static CSJObjectID alloc();

private:
    static std::atomic<CSJObjectID> m_next_id;
};

} // namespace CSJEngine

#endif // __CSJOBJECTIDALLOC_H__