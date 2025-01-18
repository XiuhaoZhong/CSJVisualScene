#ifndef __CSJGLOBALCONTEXT_H__
#define __CSJGLOBALCONTEXT_H__

#include <thread>
#include <memory>
#include <mutex>

typedef enum {
    CSJInputType_Mouse = 0,
    CSJInputType_Key,
    CSJInputType_System
} CSJInputType;

typedef enum {
    CSJWorkMode_Editor = 0,
    CSJWorkMode_Game
} CSJWorkMode;

class CSJGlobalContext {
public:
    CSJGlobalContext();
    ~CSJGlobalContext();

    void InitWorlContext();
    void InitRenderContext();
    void InitInputContext();

    void setWorkMode(CSJWorkMode workMode);

    void run();

    void gameTick(float delta_time);

protected:
    void logicTick(float delta_time);
    void renderTick(float delta_time);

private:
    bool m_bQuit;

    std::mutex                   m_tickLock;
    std::unique_ptr<std::thread> m_workThread;

};

#endif // __CSJGLOBALCONTEXT_H__