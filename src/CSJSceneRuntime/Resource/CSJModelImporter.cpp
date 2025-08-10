#include "CSJModelImporter.h"

#include <fstream>
#include <sstream>

using namespace std;

namespace CSJEngine {

CSJRenderShapeModel::CSJRenderShapeModel() {

}

CSJRenderShapeModel::CSJRenderShapeModel(std::string modelFilePath) {
    CSJModelImporter modelImporter = CSJModelImporter();

    modelImporter.parseOBJFile(modelFilePath);

    m_iNumVertices = modelImporter.getNumVertices();

    std::vector<float> verts = modelImporter.getVertices();
    std::vector<float> tcs = modelImporter.getVertices();
    std::vector<float> normals = modelImporter.getNormals();

    m_vVertices = std::vector<glm::vec3>(m_iNumVertices, glm::vec3());
    m_vTexCoords = std::vector<glm::vec2>(m_iNumVertices, glm::vec2());
    m_vNormals = std::vector<glm::vec3>(m_iNumVertices, glm::vec3());

    for (int i = 0; i < m_iNumVertices; i++) {
        m_vVertices[i] = glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]);
        m_vTexCoords[i] = glm::vec2(tcs[i * 2], tcs[i * 2 + 1]);
        m_vNormals[i] = glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
    }
}

int CSJRenderShapeModel::getNumVertices() {
    return m_iNumVertices;
}

std::vector<glm::vec3> CSJRenderShapeModel::getVertices() {
    return m_vVertices;
}

std::vector<glm::vec2> CSJRenderShapeModel::getTexCoords() {
    return m_vTexCoords;
}

std::vector<glm::vec3> CSJRenderShapeModel::getNormals() {
    return m_vNormals;
}

void CSJModelImporter::parseOBJFile(std::string& filePath) {
    float x, y, z;
    string content;
    ifstream fileStream(filePath.c_str(), ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        if (line.compare(0, 2, "v ") == 0) {
            stringstream ss(line.erase(0, 1));
            ss >> x; ss >> y; ss >> z;
            m_vVertVals.push_back(x);
            m_vVertVals.push_back(y);
            m_vVertVals.push_back(z);
        }

        if (line.compare(0, 2, "vt") == 0) {
            stringstream ss(line.erase(0, 2));
            ss >> x; ss >> y;
            m_vStVals.push_back(x);
            m_vStVals.push_back(y);
        }

        if (line.compare(0, 2, "vn") == 0) {
            stringstream ss(line.erase(0, 2));
            ss >> x; ss >> y; ss >> z;
            m_vNormalVals.push_back(x);
            m_vNormalVals.push_back(y);
            m_vNormalVals.push_back(z);
        }

        if (line.compare(0, 2, "f ") == 0) {
            string oneCorner, v, t, n;
            stringstream ss(line.erase(0, 2));
            for (int i = 0; i < 3; i++) {
                getline(ss, oneCorner, ' ');
                stringstream ocss(oneCorner);
                getline(ocss, v, '/');
                getline(ocss, t, '/');
                getline(ocss, n, '/');

                int vertRef = (stoi(v) - 1) * 3;
                int tcRef = (stoi(t) - 1) * 2;
                int normRef = (stoi(n) - 1) * 3;

                m_vTriangleVerts.push_back(m_vVertVals[vertRef]);
                m_vTriangleVerts.push_back(m_vVertVals[vertRef + 1]);
                m_vTriangleVerts.push_back(m_vVertVals[vertRef + 2]);

                m_vTexCoords.push_back(m_vStVals[tcRef]);
                m_vTexCoords.push_back(m_vStVals[tcRef +1]);

                m_vNormals.push_back(m_vNormalVals[normRef]);
                m_vNormals.push_back(m_vNormalVals[normRef + 1]);
                m_vNormals.push_back(m_vNormalVals[normRef + 2]);
            }
        }
    }

}

int CSJModelImporter::getNumVertices() {
    return m_vTriangleVerts.size() / 3;
}

std::vector<float> CSJModelImporter::getVertices() {
    return m_vTriangleVerts;
}

std::vector<float> CSJModelImporter::getTexCoords() {
    return m_vTexCoords;
}

std::vector<float> CSJModelImporter::getNormals() {
    return m_vNormals;
}

} // namespace CSJEngine