#ifndef __CSJWORLDMANAGER_H__
#define __CSJWORLDMANAGER_H__

#include "CSJGameLevel.h"

using GameLevels = std::unordered_map<std::string, std::shared_ptr<CSJGameLevel>>;

/**
 * Manager all the game world, and there will be one world at the same time,
 * meanwhile, there will be a level/scene active at the same time.
 */
class CSJWorldManager {
public:
    CSJWorldManager();
    ~CSJWorldManager();

    void initializeWorld();

    void tick(float delta_time);

    /**
     * @brief Load a level with name.
     */
    void loadLevel(std::string level_name);

    /**
     * @brief Create a level with name.
     */
    void createLevel(std::string level_name);

    /**
     * @brief Check a level exists or not.
     */
    bool levelExist(std::string level_name);

    /**
     * @brief Delete a level with name.
     */
    void deleteLevel(std::string level_name);

protected:
    void releaseAllLevels();

    /**
     * @brief Load a fixed level for test.
     */
    void loadDefaultLevel();

private:
    bool           m_bIsWorldLoaded = false;
    
    GameLevels     m_levels;
    CSJSpGameLevel m_current_level;
};

#endif // __CSJWORLDMANAGER_H__