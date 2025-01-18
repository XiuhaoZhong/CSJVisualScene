#ifndef __CSJWORLDMANAGER_H__
#define __CSJWORLDMANAGER_H__

#include "CSJGameLevel.h"

/**
 * Manager all the game world, and there will be one world at the same time,
 * meanwhile, there will be a level/scene active at the same time.
 */
class CSJWorldManager {
public:
    CSJWorldManager();
    ~CSJWorldManager();

    void tick(float delta_time);

private:
    bool m_bIsWorldLoaded = false;
    

};

#endif // __CSJWORLDMANAGER_H__