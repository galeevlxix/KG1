#ifndef MESH_H
#define MESH_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <vector>
#include <GL/glew.h>
#include <math.h>
#include <GL/freeglut.h>

#include "Utils.h"
#include "math3d.h"
#include "texture.h"
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <glm/fwd.hpp>

using namespace glm;

struct Vertex
{
    Vector3f m_pos;
    vec2 m_tex;
    Vector3f m_normal;

    Vertex() {}

    Vertex(const Vector3f& pos, const vec2& tex, const Vector3f& normal)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = normal;
    }
};

class Mesh
{
public:
    Mesh() {};
    ~Mesh() {
        Clear();
    };
    bool LoadMesh(const std::string& Filename);
    void Render();

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();
        ~MeshEntry();

        bool Init(const std::vector<Vertex>& Vertices,
            const std::vector<unsigned int>& Indices);

        GLuint VB;
        GLuint IB;

        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };

    std::vector<MeshEntry> m_Entries;
    std::vector<Texture*> m_Textures;
};

#endif /* MESH_H */