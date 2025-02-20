#include "CSJWorldManager.h"

CSJWorldManager::CSJWorldManager() {
}

CSJWorldManager::~CSJWorldManager() {
}

void CSJWorldManager::initializeWorld() {

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
