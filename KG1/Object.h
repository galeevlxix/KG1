#ifndef OBJECT_H
#define OBJECT_H

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "camera.h"
#include "pipeline.h"
#include "texture.h"
#include "math3d.h"
#include "Utils.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

using namespace glm;
using namespace std;

struct Vertex2
{
    Vector3f m_pos;
    vec2 m_tex;
    Vector3f m_normal;
    

    Vertex2() {}

    Vertex2(Vector3f pos, vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = Vector3f(0.0f, 0.0f, 0.0f);
    }
};

class Object
{
public:
    GLuint VBO;
    GLuint IBO;
    Texture* texture;

    void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex2* pVertices, unsigned int VertexCount);
};

class Cube : public Object {
public:
    void CreateBuffer();
    void Render();
};

class Floor : public Object {
public:
    void CreateBuffer();
    void Render();
};
#endif
