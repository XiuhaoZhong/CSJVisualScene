#ifndef __RENDEROBJECTS_H__
#define __RENDEROBJECTS_H__

#include <string>

#include "glm/glm.hpp"

/*
 * @brief Mesh base info.
 */
struct RenderObjMeshDesc {
    std::string mesh_file;
};

/**
 * @brief render material info.
 */
struct RenderObjMaterialDesc {
    std::string base_color_texture_file;
    std::string metallic_roughness_texture_file;
    std::string normal_texture_file;
    std::string occlusion_texture_file;
    std::string emissive_texture_file;
    bool        with_texture = false;
};

/**
 * @brief render transform info.
 */
struct RenderObjTransformDesc {
    glm::mat4 transform_matrix;
};

struct GameObjPartDesc {
    RenderObjMeshDesc mesh_desc;
    RenderObjMaterialDesc material_desc;
    RenderObjTransformDesc tranform_desc;
};

#endif // __RENDEROBJECTS_H__