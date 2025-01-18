#ifndef __CSJGAMELEVEL_H__
#define __CSJGAMELEVEL_H__

#include <unordered_map>

#include "CSJSceneRuntime/function/framework/Object/CSJObject.h"

using GameLevelObjects = std::unordered_map<CSJObjectID, CSJSpObject>;

class CSJGameLevel {
public:
    CSJGameLevel();
    ~CSJGameLevel();

    void tick(float delta_time);

private:
    GameLevelObjects m_objects;
};

#endif // __CSJGAMELEVEL_H__