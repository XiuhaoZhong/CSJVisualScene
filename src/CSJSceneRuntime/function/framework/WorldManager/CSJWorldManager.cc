#include "CSJWorldManager.h"

#include "CSJSceneRuntime/function/framework/GameLevel/CSJGameLevel.h"

namespace CSJEngine {

CSJWorldManager::CSJWorldManager() {
    loadDefaultLevel();
}

CSJWorldManager::~CSJWorldManager() {
}

void CSJWorldManager::initializeWorld() {

}

void CSJWorldManager::loadDefaultLevel() {
    // Load a default level to test the functionality.
    if (m_current_level) {
        return ;
    }

    m_current_level = std::make_shared<CSJGameLevel>();
    m_current_level->loadLevel();
}

void CSJWorldManager::tick(float delta_time) {
    if (!m_current_level) {
        return ;
    }

    m_current_level->tick(delta_time);
}

void CSJWorldManager::loadLevel(std::string level_name) {

}

void CSJWorldManager::createLevel(std::string level_name) {

}

bool CSJWorldManager::levelExist(std::string level_name) {
    return false;
}

void CSJWorldManager::deleteLevel(std::string level_name) {

}

void CSJWorldManager::releaseAllLevels() {

}

} // namespace CSJEngine
