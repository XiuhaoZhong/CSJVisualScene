#ifndef __CSJRENDERCORE_H__
#define __CSJRENDERCORE_H__

/**
 * Core of renderer, tick function is renderer's main enter.
 */
class CSJRenderCore {
public:
    CSJRenderCore();
    ~CSJRenderCore();

    void tick(float delta_time);
};

#endif // __CSJRENDERCORE_H__