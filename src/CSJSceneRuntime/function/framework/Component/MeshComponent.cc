#include "MeshComponent.h"

MeshComponent::MeshComponent() {

}

MeshComponent::~MeshComponent() {

}

void MeshComponent::tick(float delta_time) {

}

void MeshComponent::loadResource(std::weak_ptr<CSJObject> parent_object) {
    m_parent_object = parent_object;
}
