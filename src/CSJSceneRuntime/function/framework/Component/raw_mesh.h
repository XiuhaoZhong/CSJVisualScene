#ifndef __RAWMESH_H__
#define __RAWMESH_H__

#include <cstdint>
#include <string>
#include <vector>

enum class PrimitiveType {
    point,
    line,
    triangle,
    quad
};

struct RawVertexBuffer {
    uint32_t vertex_count;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> uvs;
};

struct RawIndexBuffer {
    PrimitiveType primitive_type = PrimitiveType::triangle;
    uint32_t primitive_count = 0;
    std::vector<uint32_t> indices;
};

struct MaterialTexture {
    std::string base_color;
    std::string metallic_roughness;
    std::string normal;
};

struct StaticMeshData {
    RawVertexBuffer vertex_buffer;
    RawIndexBuffer index_buffer;
    MaterialTexture material_texture;
};

#endif // __RAWMESH_H__