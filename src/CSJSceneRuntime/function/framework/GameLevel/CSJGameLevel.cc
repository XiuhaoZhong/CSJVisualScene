#include "CSJGameLevel.h"

namespace CSJEngine {

CSJGameLevel::CSJGameLevel() {
}

CSJGameLevel::~CSJGameLevel() {
}

void CSJGameLevel::tick(float delta_time) {
    if (m_objects.size() == 0) {
        return ;
    }

    for (auto pair : m_objects) {
        if (!(pair.second)) {
            continue;
        }

        pair.second->tick(delta_time);

        // TODO: physical engine tick;
    }
}

}