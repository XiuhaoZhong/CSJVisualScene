#ifndef __CSJGAMELEVEL_H__
#define __CSJGAMELEVEL_H__

#include <memory>
#include <string>
#include <unordered_map>

#include "CSJSceneRuntime/function/framework/Object/CSJObject.h"

namespace CSJEngine{

using GameLevelObjects = std::unordered_map<CSJObjectID, CSJSpObject>;

class CSJGameLevel {
public:
    CSJGameLevel();
    ~CSJGameLevel();

    void tick(float delta_time);

    /**
     * @brief Current game level is saved or not. 
     *        In the edit mode, you should reload/delete/switch level if 
     *        current level isn't saved. 
     */
    bool isSave() const {
        return m_bIsSave;
    }

    void loadLevel();

private:
    std::string m_name;
    bool        m_bIsSave = false;

    GameLevelObjects m_objects;
};

using CSJSpGameLevel = std::shared_ptr<CSJGameLevel>;

} // namespace CSJEngine

#endif // __CSJGAMELEVEL_H__