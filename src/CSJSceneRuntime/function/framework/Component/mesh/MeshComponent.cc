#include "MeshComponent.h"

namespace CSJEngine {

MeshComponent::MeshComponent() {

}

MeshComponent::~MeshComponent() {

}

void MeshComponent::tick(float delta_time) {

}

void MeshComponent::loadResource(std::weak_ptr<CSJObject> parent_object) {
    m_pParentObject = parent_object;
}

} // namespace CSJEngine 