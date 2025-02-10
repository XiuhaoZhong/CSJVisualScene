#include "CSJRenderObjectData.h"

static float toRadians(double degrees) {
    return (degrees * 2.0f * 3.14159f) / 360.0f;
}

CSJRenderMesh createSphere(int prec) {
    CSJRenderMesh sphereMesh;

    sphereMesh.numVertices = (prec + 1) * (prec + 1);
    sphereMesh.numIndices = prec * prec * 6;

    int numVertices = (prec + 1) * (prec + 1);
    int numIndices = prec * prec * 6;

    sphereMesh.vertices.resize(sphereMesh.numVertices, glm::vec3());
    sphereMesh.texCoords.resize(sphereMesh.numVertices, glm::vec2());
    sphereMesh.normals.resize(sphereMesh.numVertices, glm::vec3());
    sphereMesh.tTangents.resize(sphereMesh.numVertices, glm::vec3());

    sphereMesh.indices.resize(sphereMesh.numIndices);

    // for (int i = 0; i < numVertices; i++) {
    //     vertices.push_back(glm::vec3());
    //     texCoords.push_back(glm::vec2());
    //     normals.push_back(glm::vec3());
    //     tTangents.push_back(glm::vec3());
    // }

    // for (int i = 0; i < numIndices; i++) {
    //     indices.push_back(0);
    // }

    for (int i = 0; i <= prec; i++) {
        for (int j = 0; j <= prec; j++) {
            float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
            float x = -(float)cos(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
            float z = (float)sin(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));

            sphereMesh.vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
            sphereMesh.texCoords[i * (prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
            sphereMesh.normals[i * (prec + 1) + j] = glm::vec3(x, y, z);

            // calculate tangent vector
            if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
                sphereMesh.tTangents[i * (prec + 1) + j] = glm::vec3(0.0f, 0.0f, -1.0f);
            } else {
                sphereMesh.tTangents[i * (prec + 1) + j] = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
            }
        }
    }

    // calculate triangle indices
    for (int i = 0; i < prec; i++) {
        for (int j = 0; j < prec; j++) {
            sphereMesh.indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
            sphereMesh.indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
            sphereMesh.indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
            sphereMesh.indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
            sphereMesh.indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
            sphereMesh.indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
        }
    }

    return sphereMesh;
}

CSJRenderMesh createTorus(int prec, float fOuter, float fInner) {
    CSJRenderMesh TorusMesh;

    TorusMesh.numVertices = (prec + 1) * (prec + 1);
    TorusMesh.numIndices = prec * prec * 6;

    TorusMesh.vertices.resize(TorusMesh.numVertices, glm::vec3());
    TorusMesh.texCoords.resize(TorusMesh.numVertices, glm::vec2());
    TorusMesh.normals.resize(TorusMesh.numVertices, glm::vec3());
    TorusMesh.tTangents.resize(TorusMesh.numVertices, glm::vec3());
    TorusMesh.sTangents.resize(TorusMesh.numVertices, glm::vec3());

    TorusMesh.indices.resize(TorusMesh.numIndices);

    // for (int i = 0; i < numVertices; i++) {
    //     vertices.push_back(glm::vec3());
    //     texCoords.push_back(glm::vec2());
    //     normals.push_back(glm::vec3());
    //     sTangents.push_back(glm::vec3());
    //     tTangents.push_back(glm::vec3());
    // }

    // for (int i = 0; i < numIndices; i++) {
    //     indices.push_back(0);
    // }

    // the first ring.
    for (int i = 0; i < prec + 1; i++) {
        float amt = toRadians(i * 360.0f / prec);

        glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 pos(rMat * glm::vec4(fOuter, 0.0f, 0.0f, 1.0f));

        TorusMesh.vertices[i] = glm::vec3(pos + glm::vec3(fInner, 0.0f, 0.0f));
        TorusMesh.texCoords[i] = glm::vec2(0.0f, (float)i / (float)prec);

        rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
        TorusMesh.tTangents[i] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));

        TorusMesh.sTangents[i] = glm::vec3(glm::vec3(0.0f, 0.0f, -1.0f));

        TorusMesh.normals[i] = glm::cross(TorusMesh.tTangents[i], TorusMesh.sTangents[i]);
    }

    // rest of the ring.
    for (int i = 1; i < prec + 1; i++) {
        for (int j = 0; j < prec + 1; j++) {
            float amt = toRadians((float)i * 360.0f / prec);

            glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            TorusMesh.vertices[i * (prec + 1) + j] = glm::vec3(rMat * glm::vec4(TorusMesh.vertices[j], 1.0f));

            TorusMesh.texCoords[i * (prec + 1) + j] = glm::vec2((float)i * 2.0f / (float)prec, TorusMesh.texCoords[j].t);
            if (TorusMesh.texCoords[i * (prec + 1) + j].s > 1.0) {
                TorusMesh.texCoords[i * (prec + 1) + j].s -= 1.0f;
            }

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            TorusMesh.sTangents[i * (prec + 1) + j] = glm::vec3(rMat * glm::vec4(TorusMesh.sTangents[j], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            TorusMesh.tTangents[i * (prec + 1) + j] = glm::vec3(rMat * glm::vec4(TorusMesh.tTangents[j], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            TorusMesh.normals[i * (prec + 1) + j] = glm::vec3(rMat * glm::vec4(TorusMesh.normals[j], 1.0f));
        }
    }

    for (int ring = 0; ring < prec; ring++) {
        for (int i = 0; i < prec; i++) {
            TorusMesh.indices[((ring * prec + i) * 2) * 3 + 0] = ring * (prec + 1) + i;
            TorusMesh.indices[((ring * prec + i) * 2) * 3 + 1] = (ring + 1) * (prec + 1) + i;
            TorusMesh.indices[((ring * prec + i) * 2) * 3 + 2] = ring * (prec + 1) + i + 1;
            TorusMesh.indices[((ring * prec + i) * 2 + 1) * 3 + 0] = ring * (prec + 1) + i + 1;
            TorusMesh.indices[((ring * prec + i) * 2 + 1) * 3 + 1] = (ring + 1) * (prec + 1) + i;
            TorusMesh.indices[((ring * prec + i) * 2 + 1) * 3 + 2] = (ring + 1) * (prec + 1) + i + 1;
        }
    }

    return TorusMesh;
}

CSJRenderMesh createPyrimad() {
    return CSJRenderMesh();
}