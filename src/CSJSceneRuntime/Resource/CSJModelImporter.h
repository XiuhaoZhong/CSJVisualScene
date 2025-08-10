#ifndef __CSJMODELIMPORTER_H__
#define __CSJMODELIMPORTER_H__

#include <string>
#include <vector>

#include "glm/glm.hpp"

namespace CSJEngine {

class CSJRenderShapeModel {
public:
    CSJRenderShapeModel();
    CSJRenderShapeModel(std::string modelFilePath);

    int getNumVertices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTexCoords();
    std::vector<glm::vec3> getNormals();

private:
    int m_iNumVertices;
    std::vector<glm::vec3> m_vVertices;
    std::vector<glm::vec2> m_vTexCoords;
    std::vector<glm::vec3> m_vNormals;
};

class CSJModelImporter {
public:
    CSJModelImporter() = default;

    void parseOBJFile(std::string& filePath);
    int getNumVertices();
    std::vector<float> getVertices();
    std::vector<float> getTexCoords();
    std::vector<float> getNormals();

private:
    std::vector<float> m_vVertVals;
    std::vector<float> m_vTriangleVerts;
    std::vector<float> m_vTexCoords;
    std::vector<float> m_vStVals;
    std::vector<float> m_vNormals;
    std::vector<float> m_vNormalVals;
};

} // namespace CSJEngine
#endif // __CSJMODELIMPORTER_H__