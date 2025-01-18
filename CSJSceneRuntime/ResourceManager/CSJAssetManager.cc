#include "CSJAssetManager.h"

#include "CSJModelImporter.h"

CSJRenderMesh CSJAssetManager::loadMeshFromFile(std::string& filePath) {
    CSJRenderMesh renderMesh;

    CSJModelImporter modelImpoter;
    modelImpoter.parseOBJFile(filePath);

    renderMesh.numVertices = modelImpoter.getNumVertices();
    renderMesh.vertices.resize(renderMesh.numVertices);

    for (size_t i = 0; i < modelImpoter.getVertices().size(); i += 3) {
        renderMesh.vertices[i] = glm::vec3(modelImpoter.getVertices()[i], 
                                           modelImpoter.getVertices()[i + 1], 
                                           modelImpoter.getVertices()[2]);
    }

    for (size_t i = 0; i < modelImpoter.getTexCoords().size(); i += 2) {
        renderMesh.texCoords[i] = glm::vec2(modelImpoter.getTexCoords()[i],
                                            modelImpoter.getTexCoords()[i + 1]);
    }

    return CSJRenderMesh();
}

CSJRenderMaterial CSJAssetManager::loadMaterialFromFile(std::string& filePath) {
    return CSJRenderMaterial();
}