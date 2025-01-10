#ifndef __CSJRENDEROBJECTdATA_H__
#define __CSJRENDEROBJECTdATA_H__

#include <vector>
#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "CSJModelImporter.h"

using GraphicVec3Array = std::vector<glm::vec3>;
using GraphicVec2Array = std::vector<glm::vec2>;
using GraphicIntArray = std::vector<int>;

typedef enum {
    CSJSceneType_Point = 0,
    CSJSceneType_Shadow
} CSJPresetSceneType;

/**
 * Basic material, which is used in the Blinn-phone shader model.
 */
struct CSJRenderMaterial {
    float ambient[4];
    float diffues[4];
    float spcular[4];
    float shininess;
};

/**
 * Represent a entity could be rendered, including vertexs, normal, texture coordinates
 * and material infomations.
 */
struct CSJRenderMesh {
    GraphicIntArray  indices;        // vertexes' index vector
    GraphicVec3Array vertices;       // vertex vector
    GraphicVec2Array texCoords;      // texture coordinates vector
    GraphicVec3Array normals;        // Normal vector 
    GraphicVec3Array sTangents;      // Horizontal tangent vectors
    GraphicVec3Array tTangents;      // Vertical tangent vectors

    int              numVertices;
    int              numIndices;
};

const CSJRenderMaterial CSJSilverMateial = {
    {0.1923f, 0.1923f, 0.1923f, 1},
    {0.5075f, 0.5075f, 0.5075f, 1},
    {0.5083f, 0.5083f, 0.5083f, 1},
    51.2f
};

const CSJRenderMaterial CSJBronzeMaterial = {
    {0.2125f, 0.1275f, 0.0540f, 1},
    {0.7140f, 0.4284f, 0.1814f, 1},
    {0.3936f, 0.2719f, 0.1667f, 1},
    25.6f
};

const CSJRenderMaterial CSJGlodMaterial = {
    {0.2473f, 0.1995f, 0.0745f, 1},
    {0.7516f, 0.6065f, 0.2265f, 1},
    {0.6283f, 0.5559f, 0.3661f, 1},
    51.2f
};

/**
 * Light source
 */
struct CSJLightSource {
    float ambinet[4];
    float diffuse[4];
    float specular[4];
};

const CSJLightSource CSJWhiteLight = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f }
};

// create specified mesh;
CSJRenderMesh createSphere(int prec);
CSJRenderMesh createTorus(int prec, float fOuter, float fInner);
CSJRenderMesh createPyrimad();


#endif // __CSJRENDEROBJECTdATA_H__