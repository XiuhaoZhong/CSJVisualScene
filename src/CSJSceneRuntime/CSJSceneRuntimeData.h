#ifndef __CSJSCENERUNTIMEDATA_H__
#define __CSJSCENERUNTIMEDATA_H__

#include <QVulkanWindow>

class CSJSceneRumtimeData {
public:
    static void setVulkanInstance(QVulkanInstance *instance) {
        m_pVulkanInstance = instance;
    }

    static QVulkanInstance* getVulkanInstance() {
        return m_pVulkanInstance;
    }
private:
    static QVulkanInstance *m_pVulkanInstance;
};

#endif // __CSJSCENERUNTIMEDATA_H__